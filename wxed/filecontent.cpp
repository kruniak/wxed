#include "filecontent.h"

#include <fstream>
#include <iterator>
#include <cstdint>
#include <cctype>

#include "common.h"
#include "input.h"
#include "utils.h"

FileContent::FileContent(const std::filesystem::path& file_path)
  : Panel(WX_UIWindowNames[WX_UIWindow::FileContent], 1, 1, utils::get_curses_max_x() - 1, utils::get_curses_max_y() - 2, WX_UIColor::FileContent)
{
  std::ifstream input{ file_path, std::ios::binary };
  //m_file_bytes = std::vector<char>();

  //auto file_size = input.tellg();
  //m_file_bytes.reserve(file_size);

  //input.seekg({ 0 });

  //m_file_bytes.assign(std::istreambuf_iterator<char>(input), std::istream_iterator<char>());

  m_file_bytes = std::vector<unsigned char>({
    std::istreambuf_iterator(input),
    std::istreambuf_iterator<char>()
    });
}

void FileContent::render()
{
  switch (m_current_format)
  {
  case OutputFormat::text:
    print_text_output();
    break;
  case OutputFormat::hexadecimal:
    print_hex_output();
    break;
  case OutputFormat::disassembly:
    throw "Not implemented";
  }
}

void FileContent::register_keybinds()
{
  auto& input_processor = InputProcessor::get_instance();

  input_processor.register_keybind('j', [&]() {
    move_position(16);
    });

  input_processor.register_keybind('k', [&]() {
    move_position(-16);
    });
}

void FileContent::print_text_output() const
{
  bool exit_loop = false;
}

void FileContent::print_hex_output() const
{
  bool exit_loop = false;

  for (uint64 i = m_position; i < m_position + (utils::get_curses_max_y() - 2) * 16; i += 16)
  {
    // print bytes in hex
    unsigned hex_position_offset = 11;
    for (uint64 j = 0; j < 16; j++)
    {
      // check if we reached end of data. if so, set
      // exit_loop to true then break
      if (i + j > m_file_bytes.size() - 1)
      {
        exit_loop = true;
        break;
      }

      print_at(hex_position_offset, (i - m_position) / 16, "%02x", m_file_bytes[i + j]);
      hex_position_offset += 3;
    }

    // print alphanumeric characters
    unsigned ascii_position_offset = 60;
    for (uint64 j = 0; j < 16; j++)
    {
      // don't reach end of data. if we have, we already set
      // exit_loop earlier so just break
      if (i + j > m_file_bytes.size() - 1)
      {
        break;
      }

      const char ascii_character = std::isalnum(m_file_bytes[i + j]) ? m_file_bytes[i + j] : '.';
      print_at(ascii_position_offset, (i - m_position) / 16, "%c", ascii_character);
      ascii_position_offset++;
    }

    // print current position in hex
    print_at(0, (i - m_position) / 16, "%08x:", i);

    if (exit_loop)
    {
      break;
    }
  }
}

void FileContent::move_position(const int64 offset)
{
  if (offset < 0)
  {
    // ugly cast required, otherwise unsigned int result can overflow below zero
    if (static_cast<int64>(m_position) + offset < 0)
    {
      return;
    }
  }
  else
  {
    // TODO: fix this
    if (m_position + offset > m_file_bytes.size() + 16)
    {
      return;
    }
  }

  m_position += offset;
}

void FileContent::change_mode(Mode mode)
{
  m_current_mode = mode;
}

