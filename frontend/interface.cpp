#include "crow.h"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <utility>

#include "../dist-fs/storage.hpp"

// Simulated SSD operations
std::mutex ssd_mutex;
std::vector<std::string> ssd_files;

void upload_file(const std::string &filename) {
  std::lock_guard<std::mutex> lock(ssd_mutex);
  ssd_files.push_back(filename);
  std::cout << "File uploaded: " << filename << std::endl;
}

bool delete_file(const std::string &filename) {
  std::lock_guard<std::mutex> lock(ssd_mutex);
  auto it = std::find(ssd_files.begin(), ssd_files.end(), filename);
  if (it != ssd_files.end()) {
    ssd_files.erase(it);
    std::cout << "File deleted: " << filename << std::endl;
    return true;
  }
  return false;
}

crow::json::wvalue metadata_to_json() {
  int ssd_fd = open(DEVICE_PATH, O_RDWR);
  if (ssd_fd == -1) {
    throw std::runtime_error("Error opening SSD");
  }

  // Read the metadata table
  std::vector<ssd_metadata_t> metadata_table = metadata_table_read(ssd_fd);

  // Prepare the JSON response
  crow::json::wvalue response;
  std::vector<crow::json::wvalue> files; // Create an array of JSON objects

  for (const auto &entry : metadata_table) {
    crow::json::wvalue file;
    file["name"]       = entry.filename;
    file["offset"]     = static_cast<int64_t>(entry.start_offset);
    file["size_bytes"] = static_cast<int64_t>(entry.size);
    file["size_kb"]    = entry.size / 1024;
    file["size_mb"]    = entry.size / (1024 * 1024);

    files.push_back(std::move(file)); // Add the file object to the list
  }

  response["files"] = std::move(files); // Assign the list to the "files" key
  return response;
}

int main() {
  crow::SimpleApp app;

  // Serve the static HTML file
  CROW_ROUTE(app, "/")
  ([] {
    crow::response res;
    res.code = 200;
    res.set_static_file_info(
      "index.html"); // Ensure this file exists in your project directory
    return res;
  });

  // Upload file API
  CROW_ROUTE(app, "/api/upload")
    .methods("POST"_method)([](const crow::request &req) {
      auto filename = req.url_params.get("filename");
      if (!filename) {
        return crow::response(400, "Missing 'filename' parameter");
      }
      upload_file(filename);
      return crow::response(200, "File uploaded: " + std::string(filename));
    });

  // Delete file API
  CROW_ROUTE(app, "/api/delete")
    .methods("POST"_method)([](const crow::request &req) {
      auto filename = req.url_params.get("filename");
      if (!filename) {
        return crow::response(400, "Missing 'filename' parameter");
      }
      if (delete_file(filename)) {
        return crow::response(200, "File deleted: " + std::string(filename));
      } else {
        return crow::response(404, "File not found: " + std::string(filename));
      }
    });

  // List files API
  CROW_ROUTE(app, "/api/list").methods("GET"_method)([](const crow::request &) {
    try {
      return crow::response(metadata_to_json());
    } catch (const std::exception &ex) {
      return crow::response(500, std::string("Error: ") + ex.what());
    }
  });


  // Start the server on port 2020
  app.port(2020).multithreaded().run();

  return 0;
}
