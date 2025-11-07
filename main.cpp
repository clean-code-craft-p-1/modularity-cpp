#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <filesystem>
#include <cassert>

struct BadLine {
    int line_number;
    std::string content;
};

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string trim(const std::string& str) {
    const std::string whitespace = " \t\r\n";
    const size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    
    const size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

std::string repeat_char(char ch, int count) {
    return std::string(count, ch);
}

bool is_valid_timestamp(const std::string& timestamp) {
    auto parts = split(timestamp, ':');
    return parts.size() == 3;
}

void process_batch(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: File not found." << std::endl;
        return;
    }
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    
    std::vector<double> temps;
    std::vector<std::string> timestamps;
    int errors = 0;
    std::vector<BadLine> bad_lines;
    
    for (size_t i = 0; i < lines.size(); ++i) {
        line = trim(lines[i]);
        if (line.empty()) {
            continue;
        }
        
        auto parts = split(line, ',');
        if (parts.size() != 2) {
            errors++;
            bad_lines.push_back({static_cast<int>(i), line});
            continue;
        }
        
        std::string timestamp = trim(parts[0]);
        std::string value_str = trim(parts[1]);
        
        // Validate timestamp
        if (!is_valid_timestamp(timestamp)) {
            errors++;
            bad_lines.push_back({static_cast<int>(i), line});
            continue;
        }
        
        // Parse temperature
        double temp;
        try {
            temp = std::stod(value_str);
        } catch (const std::exception&) {
            errors++;
            bad_lines.push_back({static_cast<int>(i), line});
            continue;
        }
        
        // Drop impossible temperatures
        if (temp < -100.0 || temp > 200.0) {
            errors++;
            bad_lines.push_back({static_cast<int>(i), line});
            continue;
        }
        
        temps.push_back(temp);
        timestamps.push_back(timestamp);
    }
    
    if (temps.empty()) {
        std::cout << "No valid temperature data found." << std::endl;
        return;
    }
    
    // Calculate statistics
    double max_temp = *std::max_element(temps.begin(), temps.end());
    double min_temp = *std::min_element(temps.begin(), temps.end());
    double avg_temp = std::accumulate(temps.begin(), temps.end(), 0.0) / temps.size();
    
    // Print summary
    std::cout << repeat_char('=', 60) << std::endl;
    std::cout << "Temperature Analysis Summary" << std::endl;
    std::cout << repeat_char('=', 60) << std::endl;
    std::cout << "Total readings: " << lines.size() << std::endl;
    std::cout << "Valid readings: " << temps.size() << std::endl;
    std::cout << "Errors: " << errors << std::endl;
    std::cout << repeat_char('-', 60) << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Max temperature: " << max_temp << std::endl;
    std::cout << "Min temperature: " << min_temp << std::endl;
    std::cout << "Average temperature: " << avg_temp << std::endl;
    std::cout << repeat_char('-', 60) << std::endl;
    
    // Print invalid lines (verbose)
    if (errors > 0) {
        std::cout << "Invalid lines:" << std::endl;
        for (const auto& bad : bad_lines) {
            std::cout << "  Line " << (bad.line_number + 1) << ": " << bad.content << std::endl;
        }
    }
    
    // Save report
    std::string out_name = filename + "_summary.txt";
    std::ofstream out_file(out_name);
    if (out_file.is_open()) {
        out_file << "Temperature Analysis Summary" << std::endl;
        out_file << repeat_char('=', 50) << std::endl;
        out_file << "File analyzed: " << filename << std::endl;
        out_file << "Total readings: " << lines.size() << std::endl;
        out_file << "Valid readings: " << temps.size() << std::endl;
        out_file << "Errors: " << errors << std::endl;
        out_file << std::fixed << std::setprecision(2);
        out_file << "Max temperature: " << max_temp << std::endl;
        out_file << "Min temperature: " << min_temp << std::endl;
        out_file << "Average temperature: " << avg_temp << std::endl;
        out_file << repeat_char('-', 60) << std::endl;
        
        if (errors > 0) {
            out_file << std::endl << "Invalid lines:" << std::endl;
            for (const auto& bad : bad_lines) {
                out_file << "  Line " << (bad.line_number + 1) << ": " << bad.content << std::endl;
            }
        }
        
        out_file.close();
        std::cout << "Report saved to " << out_name << std::endl;
    } else {
        std::cout << "Error saving file" << std::endl;
    }
}

int main() {
    // Generate test data file
    const std::string test_filename = "test_temps.csv";
    const std::vector<std::string> test_data = {
        "09:15:30,23.5",
        "09:16:00,24.1",
        "09:16:30,22.8",
        "09:17:00,25.3",
        "09:17:30,23.9",
        "09:18:00,24.7",
        "09:18:30,22.4",
        "09:19:00,26.1",
        "09:19:30,23.2",
        "09:20:00,25.0"
    };
    
    std::ofstream test_file(test_filename);
    if (test_file.is_open()) {
        for (const auto& data_line : test_data) {
            test_file << data_line << std::endl;
        }
        test_file.close();
    }
    
    std::cout << "Created test file: " << test_filename << std::endl;
    
    // Process the test file
    process_batch(test_filename);
    
    // Verify the summary file was created
    const std::string summary_file = test_filename + "_summary.txt";
    if (std::filesystem::exists(summary_file)) {
        std::cout << std::endl << "Summary file created: " << summary_file << std::endl;
        
        std::ifstream verify_file(summary_file);
        if (verify_file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(verify_file)),
                              std::istreambuf_iterator<char>());
            verify_file.close();
            
            assert(content.find("Total readings: 10") != std::string::npos);
            assert(content.find("Valid readings: 10") != std::string::npos);
            assert(content.find("Errors: 0") != std::string::npos);
            std::cout << "✓ Summary file contents verified" << std::endl;
        }
    }
    
    // Clean up test files
    if (std::filesystem::exists(test_filename)) {
        std::filesystem::remove(test_filename);
    }
    if (std::filesystem::exists(summary_file)) {
        std::filesystem::remove(summary_file);
    }
    
    return 0;
}