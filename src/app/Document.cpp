#include "Document.h"
#include "math/BCurve.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>

void Document::add_curve(const std::string &name, const std::vector<Vec3d> &control_points, int degree) {
    CurveItem item;
    item.name = name;
    item.curve = control_points;
    item.degree = degree;
    item.is_nurbs = false;
    curves.push_back(item);
}

void Document::add_curve(const std::string &name, const BCurve &curve) {
    CurveItem item;
    item.name = name;
    item.curve = curve;
    item.degree = curve.get_degree();
    item.is_nurbs = curve.is_nurbs();
    if (item.is_nurbs) {
        item.weights = curve.get_weights();
    }
    curves.push_back(item);
}

void Document::remove_curve(size_t index) {
    if (index < curves.size()) {
        curves.erase(curves.begin() + index);
    }
}

std::string export_to_json(const Document &doc) {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"curves\": [\n";

    for (size_t i = 0; i < doc.curves.size(); i++) {
        const auto &curve = doc.curves[i];
        ss << "    {\n";
        ss << "      \"name\": \"" << curve.name << "\",\n";
        ss << "      \"degree\": " << curve.degree << ",\n";
        ss << "      \"is_nurbs\": " << (curve.is_nurbs ? "true" : "false") << ",\n";

        // Export control points
        ss << "      \"control_points\": [\n";

        // Handle different curve types
        std::visit([&ss](auto &&c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, std::vector<Vec3d>>) {
                for (size_t j = 0; j < c.size(); j++) {
                    ss << "        [" << c[j].x() << ", " << c[j].y() << ", " << c[j].z() << "]";
                    if (j < c.size() - 1)
                        ss << ",";
                    ss << "\n";
                }
            } else if constexpr (std::is_same_v<T, BCurve>) {
                const auto &points = c.get_control_points();
                for (size_t j = 0; j < points.size(); j++) {
                    ss << "        [" << points[j].x() << ", " << points[j].y() << ", " << points[j].z() << "]";
                    if (j < points.size() - 1)
                        ss << ",";
                    ss << "\n";
                }
            }
        },
                   curve.curve);

        ss << "      ]";

        // Export weights if NURBS
        if (curve.is_nurbs && !curve.weights.empty()) {
            ss << ",\n      \"weights\": [";
            for (size_t j = 0; j < curve.weights.size(); j++) {
                ss << curve.weights[j];
                if (j < curve.weights.size() - 1)
                    ss << ", ";
            }
            ss << "]";
        }

        ss << "\n    }";
        if (i < doc.curves.size() - 1)
            ss << ",";
        ss << "\n";
    }

    ss << "  ]\n";
    ss << "}\n";
    return ss.str();
}

bool save_document(const Document &doc, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << export_to_json(doc);
    return file.good();
}

Document load_document(const std::string &filename) {
    Document doc;
    std::ifstream file(filename);
    if (!file.is_open()) {
        return doc;
    }

    // Simple JSON parsing (for a full implementation, use a JSON library)
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();

    // For now, return empty document - full JSON parsing would require a library
    // This is a placeholder for the full implementation
    return doc;
}
