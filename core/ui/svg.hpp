//
// Created by solaire on 1/27/26.
//
#pragma once
#include <cstdint>
#include <print>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

class SVG {
   public:
    SVG() = default;
    ~SVG() = default;
    SVG(const SVG&) = delete;
    SVG& operator=(const SVG&) = delete;

    enum class command_type {
        svg,
        square,
        circle,
        arc,
        ellipse,
        line,
        path,
        size
    };

    enum class d_type : uint8_t {
        M = 'M',
        L = 'L',
        H = 'H',
        V = 'V',
        Q = 'Q',
        T = 'T',
        C = 'C',
        S = 'S',
        A = 'A',
        Z = 'Z'
    };

    struct point {
        float a, b;
    };

    struct color {
        int c[4];
    };

    struct path_d {
        d_type type;
        // move to
        point M;
        // line to
        point L;
        float H;
        float V;
        // curves
        // quadratic
        point Q[2];
        point T;
        // cubic
        point C[3];
        point S[2];
    };

    struct svg_command {
        std::string name;
        std::unordered_map<std::string, std::string> props;
        command_type type;
        float x, y;
        float width, height;
        float rx, ry;
        float cx, cy;
        float r;
        float x1, y1, x2, y2;
        std::vector<point> points;

        // universal
        std::string fill;
        std::string stroke;
        float stroke_width;
        float opacity;

        std::vector<path_d> d;
    };

    // std::vector<svg_command> commands;

    std::string tag_name(const std::string& str, size_t offset) {
        size_t pos = str.find_first_of(" >\n", offset);

        if (pos == std::string::npos) {
            std::println("Error parsing svg file: tag name invalid");
        }

        return str.substr(offset, pos - offset);
    }

    std::stack<svg_command> tags;
    std::vector<svg_command> sgv_tags;

    void Parse(std::string& str) {
        std::erase_if(str, [](char x) { return x == '\n'; });

        size_t str_len = str.size();

        std::string current_tag = "";

        for (int i = 0; i < str_len; i++) {
            char& c = str[i];

            if (c == '<') {  // tag
                if (str[i + 1] == '/') {  // closing tag
                    sgv_tags.push_back(tags.top());
                    tags.pop();
                    continue;
                }

                current_tag = tag_name(str, ++i);
                tags.push({current_tag});

                i += current_tag.size();

                std::string current_prop;

                while (str[++i] != '>') {
                    if (str[i] == '/' &&
                        str[i + 1] == '>') {  // self closing tag
                        sgv_tags.push_back(tags.top());
                        tags.pop();
                    }

                    if (str[i] == '\"') {
                        std::erase_if(current_prop, [](char c) {return c == ' ' || c == '=';});

                        while (str[++i] != '\"') {
                            tags.top().props[current_prop] += str[i];
                        }

                        current_prop = "";
                        continue;
                    }

                    current_prop += str[i];
                }
            }
        }
        if (tags.size() != 0) {
            std::println("Error: malformed svg file");
        }
    }
};
