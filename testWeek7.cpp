#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include "VectorUtils.hpp"
#include "Rendering.hpp"
#include "Plane.hpp"
#include "Sphere.hpp"
#include "Material.hpp"
#include <cmath> 
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace Catch::Matchers;

std::vector<std::vector<std::array<float, 3>>> loadImage(std::string filename, uint w = 100, uint h = 100)
{
    std::vector<std::vector<std::array<float, 3>>> image;
    std::ifstream image_file;
    image_file.open(filename);
    if (image_file)
    {
        std::string line;

        // ignore header line
        std::getline(image_file, line);

        // get dimensions
        std::getline(image_file, line);
        std::istringstream line_stream(line);
        uint width, height;
        line_stream >> width;
        line_stream >> height;

        if ((width != w) || (height != h))
        {
            throw std::runtime_error("Dimensions of the image are not as expected");
        }

        image.resize(width);
        for (auto &col : image)
        {
            col.resize(height);
        }

        // ignore  pixel limit
        std::getline(image_file, line);

        for (uint y = 0; y < height; y++)
        {
            for (uint x = 0; x < width; x++)
            {
                if (std::getline(image_file, line))
                {
                    std::istringstream pixel_stream(line);
                    pixel_stream >> image[x][y][0];
                    pixel_stream >> image[x][y][1];
                    pixel_stream >> image[x][y][2];
                }
                else
                {
                    throw std::runtime_error("Ran out of pixel data.");
                }
            }
        }
    }
    else
    {
        throw std::runtime_error("File " + filename + " not found.");
    }

    return image;
}

float clamp0to255(float val)
{
    return std::max(std::min(val, float(255.)), float(0.));
}

double diffImage(std::vector<std::vector<std::array<float, 3>>> im1,
                 std::vector<std::vector<std::array<float, 3>>> im2)
{
    size_t width, height;
    width = im1.size();
    height = im1.at(0).size();

    REQUIRE(im2.size() == width);
    REQUIRE(im2.at(0).size() == height);

    double diff = 0;
    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            diff += fabs(im1[i][j][0] - clamp0to255(im2[i][j][0]));
            diff += fabs(im1[i][j][1] - clamp0to255(im2[i][j][1]));
            diff += fabs(im1[i][j][2] - clamp0to255(im2[i][j][2]));
        }
    }
    double average_diff = diff / (width * height);
    return average_diff;
}



TEST_CASE("Test Material Constructor with Reflectance", "[Test Material]")
{
    using namespace VecUtils;
    Material mat(255, 255, 255);
    REQUIRE(mat.getReflectance() == 0);

    Material mat2(255, 255, 255, 0.5);
    REQUIRE(mat2.getReflectance() == 0.5);

    Material *m;

    bool exception_thrown = false;
    try
    {
        m = new Material(255, 255, 255, -1);
    }
    catch(...)
    {
        exception_thrown = true;
    }

    REQUIRE((exception_thrown || (m->getReflectance()==0)));
  
    exception_thrown = false;
    try
    {
        m = new Material(255, 255, 255, 1.5);
    }
    catch(...)
    {
        exception_thrown = true;
    }
    REQUIRE((exception_thrown || (m->getReflectance()==1)));
}

TEST_CASE("Test Reflective Render", "[Test Reflection]")
{
    using namespace VecUtils;
    using std::vector;

    Sphere sphere(2, {4,0,5}, Material(0, 0, 255));
    Sphere sphere2(5, {0, 0, 0}, Material(255, 255, 255, 0.5));
    Camera cam(100, 100);
    
    vector<Object*> objs{&sphere, &sphere2};

    // Attempt to render
    vector<vector<std::array<float, 3>>> image_data = Render::genImage(cam, objs);

    // Load expectation
    auto expected_image = loadImage("data/SphereReflection.pbm", 100, 100);

    double average_diff = diffImage(expected_image, image_data);
    REQUIRE(average_diff < 10);
}


TEST_CASE("TestFiniteRecursion", "[Test Reflection]")
{
    using namespace VecUtils;
    using std::vector;

    Plane wall1({0, 0, 20}, {0, 0, -1}, Material(255, 255, 255, 1.0));
    Plane wall2({0, 0, -20}, {0, 0, 1}, Material(255, 255, 255, 1.0));
    Camera cam(100, 100);
    
    vector<Object*> objs{&wall1, &wall2};

    // Attempt to render
    vector<vector<std::array<float, 3>>> image_data = Render::genImage(cam, objs);
}