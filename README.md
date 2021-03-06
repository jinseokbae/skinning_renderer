# skinning_renderer
This is OpenGL(3.3) renderer for researchers who are designing neural nets to infer skin weights of given 3D model.

## Installation
Libraries below should be installed before running.
  1. assimp
  2. glfw
  3. glm
  
## Running
I tested with Ubuntu 18.04 with inputs of collada files (both of Ground Truth and prediction models)

If you want to your other collada files, edit main.cpp.

* Note : I tested for collada files with having only one mesh which is positioned in the first children node of the root. (i.e., If you want to input a collada file with multiple meshes, edit model.h file or you can use blender to merge all the meshes. In case you choose latter method, then you don't have to change codes:) )   

## Demo Video
I used model and animation from Mixamo.com
Description:
  1. "GT model" and "OURS model" &#8594; just visualizing two models
  2. "GT weights" and "OURS weights" &#8594; visualizing each vertex with bones that are influencing to it. (each bone is assigned to different color. You can check out given colors in main.cpp file)
  3. "Deformation Error" &#8594; this is eulcidean distance between same vertices of GT & OURS model. I used this notation from the paper named "Neuroskinning: Automatic skin binding for production characters with deep graph networks."
![Demo Video](https://github.com/jinseokbae/skinning_renderer/blob/master/demo_mouse.gif)

## Reference
  1. fractions of sample codes from LearnOpenGL(https://learnopengl.com/)
  2. basics of skeletal animation from blog "http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html"

## Citation
  1. Notation of deformation error from Liu, Lijuan, et al. "Neuroskinning: Automatic skin binding for    production characters with deep graph networks." ACM Transactions on Graphics (TOG) 38.4 (2019): 1-12.
