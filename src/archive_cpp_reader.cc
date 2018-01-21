/** MIT License

 Copyright (c) 2018 leimiaos

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */
#include "archive_cpp_reader.h"

#include "archive.h"
#include "archive_entry.h"
#include "archive_cpp_entry.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

namespace {
  class ArchiveBuf: public std::streambuf
  {
  public:
    ArchiveBuf(struct archive* ar);
    ~ArchiveBuf();

    void reset();

  protected:
    int underflow();

  private:
    struct archive* ar;
    char *ptr;
  };

  ArchiveBuf::ArchiveBuf(struct archive* ar)
  {
    this->ar = ar;
    this->ptr = new char[BUFFER_SIZE];

    setg(ptr, ptr, ptr);
  }

  ArchiveBuf::~ArchiveBuf()
  {
    delete[] this->ptr;
  }

  int ArchiveBuf::underflow()
  {
    int r = archive_read_data(ar, ptr, BUFFER_SIZE);
    if (r < 0) {
      throw std::runtime_error("Unknown error");
    } else if (r == 0) {
      return std::streambuf::traits_type::eof();
    } else {
      setg(ptr, ptr, ptr + r);
    }
    return std::streambuf::traits_type::to_int_type(*ptr);
  }

  void ArchiveBuf::reset()
  {
    setg(ptr, ptr, ptr);
  }
}

namespace archive_cpp {
  struct ArchiveReader_impl
  {
    ArchiveReader_impl();
    ~ArchiveReader_impl();

    void open(const std::string& archive_name);

    bool NextFile();
    void MoveToEntry(const std::string& path);
    void init_stream();

    archive* ar;
    ArchiveBuf* buf;
    ArchiveEntry entry_info;
    std::istream* stream;
    enum {
      Failed,
      Initialized,
      ArchiveOpen,
      ContentOpen,
      EndOfArchive
    } status;
  };

  ArchiveReader::ArchiveReader(const std::string& archive_file)
  {
    this->impl = new ArchiveReader_impl();
    this->impl->open(archive_file);
  }

  ArchiveReader::ArchiveReader(const std::string& archive_file, const std::string& content_name)
  {
    this->impl = new ArchiveReader_impl();
    this->impl->open(archive_file);
    this->impl->MoveToEntry(content_name);
  }

  ArchiveReader::~ArchiveReader()
  {
    if (this->impl) {
      delete this->impl;
      this->impl = NULL;
    }
  }

  bool ArchiveReader::NextFile()
  {
    return impl->NextFile();
  }

  std::istream& ArchiveReader::GetStream()
  {
    if (!impl->stream) {
      impl->init_stream();
    }
    return *impl->stream;
  }

  const ArchiveEntry* ArchiveReader::EntryInfo() const
  {
    return &impl->entry_info;
  }

  ArchiveReader_impl::ArchiveReader_impl()
  {
    ar = NULL;
    buf = NULL;
    stream = NULL;
    status = Initialized;

    ar = archive_read_new();
    archive_read_support_filter_all(ar);
    archive_read_support_format_all(ar);

    buf = new ArchiveBuf(ar);
  }

  ArchiveReader_impl::~ArchiveReader_impl()
  {
    if (ar) {
      archive_read_free(ar);
    }
    if (buf) {
      delete buf;
    }
    if (stream) {
      delete stream;
    }
  }

  void ArchiveReader_impl::open(const std::string& archive_file)
  {
    int r = archive_read_open_filename(ar, archive_file.c_str(), 10240);
    if (r != ARCHIVE_OK) {
      throw std::runtime_error("Cannot open " + archive_file);
    }
    status = ArchiveOpen;
  }

  bool ArchiveReader_impl::NextFile()
  {
    if (stream) {
      delete stream;
      stream = NULL;
    }
    if (status == Failed || status == Initialized || status == EndOfArchive) {
      return false;
    }

    struct archive_entry *entry;

    entry_info.file_path.clear();

    int r = archive_read_next_header(ar, &entry);

    if (r != ARCHIVE_OK) {
      status = EndOfArchive;
      return false;
    }
    entry_info.file_path = archive_entry_pathname(entry);

    status = ContentOpen;
    return r == ARCHIVE_OK;
  }

  void ArchiveReader_impl::MoveToEntry(const std::string& content_name)
  {
    while (NextFile()) {
      if (entry_info.file_path == content_name) {
        return;
      }
    }
    throw std::runtime_error("Cannot open " + content_name);
  }

  void ArchiveReader_impl::init_stream()
  {
    if (stream) {
      delete stream;
      stream = NULL;
    }
    buf->reset();
    if (status != ContentOpen) {
      throw std::runtime_error("Entry is not open");
    }

    stream = new std::istream(buf);
  }

}
