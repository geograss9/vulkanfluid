project_path=/media/gobong/ku.config/projects.with.this.device/vulkflub_done

/usr/bin/glslc "${project_path}"/shader_Sourcing.comp -o shader_Sourcing_comp.spv
/usr/bin/glslc "${project_path}"/shader_Divergence.comp -o shader_Divergence_comp.spv
/usr/bin/glslc "${project_path}"/shader_Jacobi_1.comp -o shader_Jacobi_1_comp.spv
/usr/bin/glslc "${project_path}"/shader_Jacobi_2.comp -o shader_Jacobi_2_comp.spv
/usr/bin/glslc "${project_path}"/shader_ApplyPressure.comp -o shader_ApplyPressure_comp.spv
/usr/bin/glslc "${project_path}"/shader_ComputeVorticity.comp -o shader_ComputeVorticity_comp.spv
/usr/bin/glslc "${project_path}"/shader_ApplyVorticity.comp -o shader_ApplyVorticity_comp.spv
/usr/bin/glslc "${project_path}"/shader_Advection.comp -o shader_Advection_comp.spv

/usr/bin/glslc "${project_path}"/shader.vert -o shader_vert.spv
/usr/bin/glslc "${project_path}"/shader.frag -o shader_frag.spv

/usr/bin/glslc "${project_path}"/shader2.vert -o shader2_vert.spv
/usr/bin/glslc "${project_path}"/shader2.frag -o shader2_frag.spv
	# I think glslc doesn't support 'spv output file' path redirection.

g++ -o "${project_path}"/main "${project_path}"/main.cpp -L/usr/lib/x86_64-linux-gnu -lvulkan -lglfw 
#-L/usr/include/stb -lstb



"${project_path}"/main