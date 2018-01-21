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

#include <iostream>
#include <fstream>

#include "archive_cpp.h"

void usage()
{
  std::cerr << "usage: extractor zipfile [content]" << std::endl;
}

int main(int argc, char** argv)
{
  archive_cpp::ArchiveReader* reader;
  if (argc == 2) {
    reader = new archive_cpp::ArchiveReader(argv[1]);
  } else if (argc == 3) {
    reader = new archive_cpp::ArchiveReader(argv[1], argv[2]);
  } else {
    usage();
    return 2;
  }
  while (reader->NextFile()) {
    std::cout << "Extract file: " << reader->EntryInfo()->file_path << std::endl;
    std::ofstream fout(reader->EntryInfo()->file_path.c_str());
    std::copy(std::istream_iterator<char>(reader->GetStream()), std::istream_iterator<char>(), std::ostream_iterator<char>(fout));
  }
  delete reader;
  return 0;
}
