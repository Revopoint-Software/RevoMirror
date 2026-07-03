/**
 * @file file_handler.cpp
 * @brief Definitions for file handling functions.
 */

// standard includes
#include <filesystem>
#include <fstream>

// local includes
#include "file_handler.h"
#include "logging.h"


#include <nlohmann/json.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree::json_parser;

namespace file_handler {
  std::string get_parent_directory(const std::string &path) {
    // remove any trailing path separators
    std::string trimmed_path = path;
    while (!trimmed_path.empty() && trimmed_path.back() == '/') {
      trimmed_path.pop_back();
    }

    std::filesystem::path p(trimmed_path);
    return p.parent_path().string();
  }

  bool make_directory(const std::string &path) {
    // first, check if the directory already exists
    if (std::filesystem::exists(std::filesystem::path(path))) {
      return true;
    }

    return std::filesystem::create_directories(std::filesystem::path(path));
  }

  std::string read_file(const char *path) {
    if (!std::filesystem::exists(std::filesystem::path(path))) {
      BOOST_LOG(debug) << "Missing file: " << path;
      return {};
    }

    std::ifstream in{std::filesystem::path(path)};
    return std::string {(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()};
  }

  int write_file(const char *path, const std::string_view &contents) {
    std::ofstream out{std::filesystem::path(path)};

    if (!out.is_open()) {
      return -1;
    }

    out << contents;

    return 0;
  }

  void read_json(const std::string &filename, pt::ptree &root)
  {
    std::basic_ifstream<pt::ptree::key_type::value_type> stream(std::filesystem::path(filename.c_str()));
    if (!stream)
        BOOST_PROPERTY_TREE_THROW(json_parser_error("file_handler read_json cannot open file", filename, 0));
    stream.imbue(std::locale());

    pt::read_json(stream, root);
  }

  void write_json(const std::string &filename, const pt::ptree &root)
  {
    std::basic_ofstream<pt::ptree::key_type::value_type> stream(std::filesystem::path(filename.c_str()));
    if (!stream)
        BOOST_PROPERTY_TREE_THROW(json_parser_error("file_handler write_json cannot open file", filename, 0));
    stream.imbue(std::locale());

    pt::write_json(stream, root);
  }

  bool exists(const fs::path& filename, std::error_code& ec)
  {
    return fs::exists(std::filesystem::path(filename.c_str()), ec);
  }

  bool exists(const fs::path& filename)
  {
    return fs::exists(std::filesystem::path(filename.c_str()));
  }
}  // namespace file_handler
