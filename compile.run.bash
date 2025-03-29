project_path="."

rm -rf "${project_path}"/shaders_out
mkdir "${project_path}"/shaders_out

/usr/bin/glslc "${project_path}"/shaders/shader_Sourcing.comp -o shaders_out/shader_Sourcing_comp.spv
/usr/bin/glslc "${project_path}"/shaders/shader_Divergence.comp -o shaders_out/shader_Divergence_comp.spv
/usr/bin/glslc "${project_path}"/shaders/shader_Jacobi_1.comp -o shaders_out/shader_Jacobi_1_comp.spv
/usr/bin/glslc "${project_path}"/shaders/shader_Jacobi_2.comp -o shaders_out/shader_Jacobi_2_comp.spv
/usr/bin/glslc "${project_path}"/shaders/shader_ApplyPressure.comp -o shaders_out/shader_ApplyPressure_comp.spv
/usr/bin/glslc "${project_path}"/shaders/shader_ComputeVorticity.comp -o shaders_out/shader_ComputeVorticity_comp.spv
/usr/bin/glslc "${project_path}"/shaders/shader_ApplyVorticity.comp -o shaders_out/shader_ApplyVorticity_comp.spv
/usr/bin/glslc "${project_path}"/shaders/shader_Advection.comp -o shaders_out/shader_Advection_comp.spv

/usr/bin/glslc "${project_path}"/shaders/shader.vert -o shaders_out/shader_vert.spv
/usr/bin/glslc "${project_path}"/shaders/shader.frag -o shaders_out/shader_frag.spv

/usr/bin/glslc "${project_path}"/shaders/shader2.vert -o shaders_out/shader2_vert.spv
/usr/bin/glslc "${project_path}"/shaders/shader2.frag -o shaders_out/shader2_frag.spv

g++ -o "${project_path}"/main "${project_path}"/main.cpp -L/usr/lib/x86_64-linux-gnu -lvulkan -lglfw 

"${project_path}"/main