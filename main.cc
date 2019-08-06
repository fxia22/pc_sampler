#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <cstdio>
#include <string>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <math.h>  
#include <random>


static std::string GetBaseDir(const std::string& filepath) {
  if (filepath.find_last_of("/\\") != std::string::npos)
    return filepath.substr(0, filepath.find_last_of("/\\"));
  return "";
}


static bool FileExists(const std::string& abs_filename) {
  bool ret;
  FILE* fp = fopen(abs_filename.c_str(), "rb");
  if (fp) {
    ret = true;
    fclose(fp);
  } else {
    ret = false;
  }

  return ret;
}


int main(int argc, char ** argv){

    int point_per_sqm = 10000;

    if (argc < 2) {
      printf("./pc_sampler mesh.obj <optional: points per square meter> > pc.xyz");
      return 1;
    }
    if (argc >= 3) {
      point_per_sqm = std::stoi(argv[2]);
    }

    printf("# pc sampler\n");
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    std::string filename(argv[1]);
    std::string base_dir = GetBaseDir(filename);
    if (base_dir.empty()) {
        base_dir = ".";
    }

    base_dir += "/";


    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(),
                              base_dir.c_str());

    printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
    printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
    printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
    printf("# of materials = %d\n", (int)materials.size());
    printf("# of shapes    = %d\n", (int)shapes.size());

    materials.push_back(tinyobj::material_t());
    unsigned char* image;
     int w, h;
      int comp;

    for (size_t i = 0; i < materials.size(); i++) {
        printf("# material[%d].diffuse_texname = %s\n", int(i),
        materials[i].diffuse_texname.c_str());
    }

    {
    for (size_t m = 0; m < materials.size(); m++) {
      tinyobj::material_t* mp = &materials[m];

      if (mp->diffuse_texname.length() > 0) {
        // Only load the texture if it is not already loaded
        {
         

          std::string texture_filename = mp->diffuse_texname;
          if (!FileExists(texture_filename)) {
            // Append base dir.
            texture_filename = base_dir + mp->diffuse_texname;
            if (!FileExists(texture_filename)) {
              std::cerr << "# Unable to find file: " << mp->diffuse_texname
                        << std::endl;
              exit(1);
            }
          }

          image = stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
          if (!image) {
            std::cerr << "# Unable to load texture: " << texture_filename
                      << std::endl;
            exit(1);
          }
          std::cout << "# Loaded texture: " << texture_filename << ", w = " << w
                    << ", h = " << h << ", comp = " << comp << std::endl;
        }
      }
    }
  }

    int s = 0; //take first shape

    for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {

        tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
        tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
        tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

        int current_material_id = shapes[s].mesh.material_ids[f];

        if ((current_material_id < 0) ||
            (current_material_id >= static_cast<int>(materials.size()))) {
          // Invaid material ID. Use default material.
          current_material_id =
              materials.size() -
              1;  // Default material is added to the last item in `materials`.
        }
        
       float tc[3][2];
        if (attrib.texcoords.size() > 0) {
          if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
              (idx2.texcoord_index < 0)) {
            // face does not contain valid uv index.
            tc[0][0] = 0.0f;
            tc[0][1] = 0.0f;
            tc[1][0] = 0.0f;
            tc[1][1] = 0.0f;
            tc[2][0] = 0.0f;
            tc[2][1] = 0.0f;
          } else {
            assert(attrib.texcoords.size() >
                   size_t(2 * idx0.texcoord_index + 1));
            assert(attrib.texcoords.size() >
                   size_t(2 * idx1.texcoord_index + 1));
            assert(attrib.texcoords.size() >
                   size_t(2 * idx2.texcoord_index + 1));

            // Flip Y coord.
            tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
            tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
            tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
            tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
            tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
            tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
          }
        } else {
          tc[0][0] = 0.0f;
          tc[0][1] = 0.0f;
          tc[1][0] = 0.0f;
          tc[1][1] = 0.0f;
          tc[2][0] = 0.0f;
          tc[2][1] = 0.0f;
        }

        //printf("%f %f %f %f %f %f\n", tc[0][0], tc[0][1], tc[1][0], tc[1][1], tc[2][0], tc[2][1]);
        
        int vi[3];      // indexes
        float v[3][3];  // coordinates
        float ni[3];
        float n[3][3];


         for (int k = 0; k < 3; k++) {
          vi[0] = idx0.vertex_index;
          vi[1] = idx1.vertex_index;
          vi[2] = idx2.vertex_index;
          
          ni[0] = idx0.normal_index;
          ni[1] = idx1.normal_index;
          ni[2] = idx2.normal_index;

          assert(vi[0] >= 0);
          assert(vi[1] >= 0);
          assert(vi[2] >= 0);

          v[0][k] = attrib.vertices[3 * vi[0] + k];
          v[1][k] = attrib.vertices[3 * vi[1] + k];
          v[2][k] = attrib.vertices[3 * vi[2] + k];
        
          n[0][k] = attrib.normals[3 * ni[0] + k];
          n[1][k] = attrib.normals[3 * ni[1] + k];
          n[2][k] = attrib.normals[3 * ni[2] + k];
        

        }

        float area;

        float l0 = sqrt((v[0][0] - v[1][0]) * (v[0][0] - v[1][0]) + (v[0][1] - v[1][1]) * (v[0][1] - v[1][1]) + (v[0][2] - v[1][2]) * (v[0][2] - v[1][2]));
        float l1 = sqrt((v[2][0] - v[1][0]) * (v[2][0] - v[1][0]) + (v[2][1] - v[1][1]) * (v[2][1] - v[1][1]) + (v[2][2] - v[1][2]) * (v[2][2] - v[1][2]));
        float l2 = sqrt((v[0][0] - v[2][0]) * (v[0][0] - v[2][0]) + (v[0][1] - v[2][1]) * (v[0][1] - v[2][1]) + (v[0][2] - v[2][2]) * (v[0][2] - v[2][2]));
        
        float s = (l0 + l1 + l2)/2;

        area = sqrt(s * (s-l0) * (s-l1) * (s-l2));
        
        int nsamples = int(area * point_per_sqm);
        printf("# area %f nsamples %d \n", area, nsamples);
        

        for (int j = 0; j < nsamples; j++) {
            float lambda_a = 1;
            float lambda_b = 1;

            while (lambda_a + lambda_b > 1) {
              lambda_a = distribution(generator);
              lambda_b = distribution(generator);
            }

            float coordx = tc[0][0] + lambda_a * (tc[1][0] - tc[0][0]) + lambda_b*(tc[2][0] - tc[0][0]);
            float coordy = tc[0][1] + lambda_a * (tc[1][1] - tc[0][1]) + lambda_b*(tc[2][1] - tc[0][1]);

            if (coordx < 0) coordx = 0;
            if (coordx > 1) coordx = 1;
            if (coordy < 0) coordy = 0;
            if (coordy > 1) coordy = 1;

            int x = int(coordx * w);
            int y = int(coordy * h);

            unsigned char r = image[comp*(y * w + x)];
            unsigned char g = image[comp*(y * w + x) + 1];
            unsigned char b = image[comp*(y * w + x) + 2];

            float px = v[0][0] + lambda_a*(v[1][0] - v[0][0]) + lambda_b*(v[2][0] - v[0][0]);
            float py = v[0][1] + lambda_a*(v[1][1] - v[0][1]) + lambda_b*(v[2][1] - v[0][1]);
            float pz = v[0][2] + lambda_a*(v[1][2] - v[0][2]) + lambda_b*(v[2][2] - v[0][2]);

            float nx = n[0][0] + lambda_a*(n[1][0] - n[0][0]) + lambda_b*(n[2][0] - n[0][0]);
            float ny = n[0][1] + lambda_a*(n[1][1] - n[0][1]) + lambda_b*(n[2][1] - n[0][1]);
            float nz = n[0][2] + lambda_a*(n[1][2] - n[0][2]) + lambda_b*(n[2][2] - n[0][2]);

            printf("%f %f %f %f %f %f %d %d %d\n", px, py, pz, nx, ny, nz, int(r), int(g), int(b));
        }

    }


    stbi_image_free(image);

    return 0;
}