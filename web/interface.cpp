#include "crow.h"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>

// Simulated SSD operations
std::mutex ssd_mutex;
std::vector<std::string> ssd_files;

void upload_file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(ssd_mutex);
    ssd_files.push_back(filename);
    std::cout << "File uploaded: " << filename << std::endl;
}

bool delete_file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(ssd_mutex);
    auto it = std::find(ssd_files.begin(), ssd_files.end(), filename);
    if (it != ssd_files.end()) {
        ssd_files.erase(it);
        std::cout << "File deleted: " << filename << std::endl;
        return true;
    }
    return false;
}

std::vector<std::string> list_files() {
    std::lock_guard<std::mutex> lock(ssd_mutex);
    return ssd_files;
}

int main() {
    crow::SimpleApp app;

    // Serve the static HTML file
    CROW_ROUTE(app, "/")
    ([] {
        crow::response res;
        res.code = 200;
        res.set_static_file_info("index.html"); // Ensure this file exists in your project directory
        return res;
    });

    // Upload file API
    CROW_ROUTE(app, "/api/upload").methods("POST"_method)([](const crow::request& req) {
        auto filename = req.url_params.get("filename");
        if (!filename) {
            return crow::response(400, "Missing 'filename' parameter");
        }
        upload_file(filename);
        return crow::response(200, "File uploaded: " + std::string(filename));
    });

    // Delete file API
    CROW_ROUTE(app, "/api/delete").methods("POST"_method)([](const crow::request& req) {
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
    CROW_ROUTE(app, "/api/list").methods("GET"_method)([](const crow::request&) {
        auto files = list_files();
        crow::json::wvalue response;
        response["files"] = files;
        return crow::response(response);
    });

    // Start the server on port 2020
    app.port(2020).multithreaded().run();

    return 0;
}

