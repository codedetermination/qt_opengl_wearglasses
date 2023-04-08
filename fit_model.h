#ifndef FIT_MODEL_H
#define FIT_MODEL_H
/*
 * eos - A 3D Morphable Model fitting library written in modern C++11/14.
 *
 * File: examples/fit-model-simple.cpp
 *
 * Copyright 2015 Patrik Huber
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "eos/core/Image.hpp"
#include "eos/core/image/opencv_interop.hpp"
#include "eos/core/Landmark.hpp"
#include "eos/core/LandmarkMapper.hpp"
#include "eos/core/read_pts_landmarks.hpp"
#include "eos/core/write_obj.hpp"
#include "eos/fitting/fitting.hpp"
#include "eos/morphablemodel/Blendshape.hpp"
#include "eos/morphablemodel/MorphableModel.hpp"
#include "eos/render/opencv/draw_utils.hpp"
#include "eos/render/texture_extraction.hpp"
#include "eos/cpp17/optional.hpp"

#include "Eigen/Core"

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace eos;
namespace po = boost::program_options;
namespace fs = boost::filesystem;
using eos::core::Landmark;
using eos::core::LandmarkCollection;
using cv::Mat;
using std::cout;
using std::endl;
using std::string;
using std::vector;


/// 从给定的图像和3D形状模型中估计3D形状和表情系数。
///
/// 该应用程序演示了从图像到3d模型的相机估计和拟合。首先，从.pts 文件加载68个关键点，并使用LandmarkMapper将其转换为顶点索引。然后，估计一个正交相机矩阵，然后使用这个相机矩阵，将形状拟合到三维模型。
/// @param[in] image 图像。

class fit_model{
public:
   //string modelfile, imagefile, landmarksfile, mappingsfile, outputbasename;
    
    string modelfile = "./obj_use/sfm_shape_3448.bin"; ///< 到模型文件的路径。
    string mappingsfile = "./obj_use/ibug_to_sfm.txt"; ///< 到模型顶点到图像点的映射文件的路径。
    string landmarksfile = "output.pts"; ///< 到图像中68个关键点的 .pts 文件的路径。
    string outputbasename = "./texture/out1"; ///< 输出文件的基本名称。
    string contourfile = "./obj_use/sfm_model_contours.json"; ///< 到模型轮廓的 JSON 文件的路径。
    string edgetopologyfile = "./obj_use/sfm_3448_edge_topology.json"; ///< 到模型边缘拓扑的 JSON 文件的路径。
    string blendshapesfile = "./obj_use/expression_blendshapes_3448.bin"; ///< 到表情 blendshapes 的文件的路径。
    cv::Mat image; ///< 输入图像。
    LandmarkCollection<Eigen::Vector2f> landmarks; ///< 从 .pts 文件加载的68个关键点。
    morphablemodel::MorphableModel morphable_model; ///< 加载的 3D 可变形模型。
    core::LandmarkMapper landmark_mapper; ///< 用于将 ibug 68点映射到模型顶点的 LandmarkMapper。

    /// @brief  从给定的图像和3D形状模型中估计3D形状和表情系数。
    /// @param image_input  输入图像。
    fit_model(cv::Mat image_input) : image(image_input){
        //cv::imshow("input",image_input);
        cv::waitKey(0);
        try
        {
            landmarks = core::read_pts_landmarks(landmarksfile);
        } catch (const std::runtime_error& e)
        {
            cout << "Error reading the landmarks: " << e.what() << endl;
            throw EXIT_FAILURE;
        }

        try
        {
            morphable_model = morphablemodel::load_model(modelfile);
        } catch (const std::runtime_error& e)
        {
            cout << "Error loading the Morphable Model: " << e.what() << endl;
            throw EXIT_FAILURE;
        }
        // The landmark mapper is used to map 2D landmark points (e.g. from the ibug scheme) to vertex ids:

        try
        {
            landmark_mapper = core::LandmarkMapper(mappingsfile);
        } catch (const std::exception& e)
        {
            cout << "Error loading the landmark mappings: " << e.what() << endl;
            throw EXIT_FAILURE;
        }

    }
    /// 生成obj文件
    /// @return 生成的obj文件储存到outputbasename中
    void generate_obj(){
        const std::vector<morphablemodel::Blendshape> blendshapes = morphablemodel::load_blendshapes(blendshapesfile);

        morphablemodel::MorphableModel morphable_model_with_expressions(
           morphable_model.get_shape_model(), blendshapes, morphable_model.get_color_model(), cpp17::nullopt,
           morphable_model.get_texture_coordinates());

        // These two are used to fit the front-facing contour to the ibug contour landmarks:
        const fitting::ModelContour model_contour =
           contourfile.empty() ? fitting::ModelContour() : fitting::ModelContour::load(contourfile);
        const fitting::ContourLandmarks ibug_contour = fitting::ContourLandmarks::load(mappingsfile);

        // The edge topology is used to speed up computation of the occluding face contour fitting:
        const morphablemodel::EdgeTopology edge_topology = morphablemodel::load_edge_topology(edgetopologyfile);
        //Mat outimg = image.clone();
        Mat outimg = image.clone();
        for (auto&& lm : landmarks)
        {
            cv::rectangle(outimg, cv::Point2f(lm.coordinates[0] - 2.0f, lm.coordinates[1] - 2.0f),
                          cv::Point2f(lm.coordinates[0] + 2.0f, lm.coordinates[1] + 2.0f), {255, 0, 0});
        }

        // Fit the model, get back a mesh and the pose:
        core::Mesh mesh;
        fitting::RenderingParameters rendering_params;
        std::tie(mesh, rendering_params) = fitting::fit_shape_and_pose(
            morphable_model_with_expressions, landmarks, landmark_mapper, image.cols, image.rows, edge_topology,
            ibug_contour, model_contour, 5, cpp17::nullopt, 30.0f);

        // The 3D head pose can be recovered as follows:
        float yaw_angle = glm::degrees(glm::yaw(rendering_params.get_rotation()));
        // and similarly for pitch and roll.

        // Extract the texture from the image using given mesh and camera parameters:
        const core::Image4u texturemap =
            render::extract_texture(mesh, rendering_params.get_modelview(), rendering_params.get_projection(),
                                    render::ProjectionType::Orthographic, core::from_mat_with_alpha(image));

        // Draw the fitted mesh as wireframe, and save the image:
        render::draw_wireframe(outimg, mesh, rendering_params.get_modelview(), rendering_params.get_projection(),
                               fitting::get_opencv_viewport(image.cols, image.rows));
        fs::path outputfile = outputbasename + ".png";
        cv::imwrite(outputfile.string(), outimg);


        // Save the mesh as textured obj:

        outputfile = outputbasename + ".obj";
        core::write_textured_obj(mesh, outputfile.string());

        // And save the texture map:
        //outputfile.replace_extension(".texture.png");
        fs::path outputfile1 = outputbasename + ".texture.png";
        cv::imwrite(outputfile1.string(), core::to_mat(texturemap));

        //throw EXIT_SUCCESS;
    }

};






#endif // FIT_MODEL_H
