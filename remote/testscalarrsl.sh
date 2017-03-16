echo "TESTING SCALAR RSL SHADERS..."
./render_wrl.sh scenes/sphere.x3dv -s rsl_normal
./render_wrl.sh scenes/sphere.x3dv -s rsl_bw
./render_wrl.sh scenes/sphere.x3dv -s rsl_diffuse
./render_wrl.sh scenes/sphere.x3dv -s rsl_specular

./render_wrl.sh scenes/sphere.x3dv -s rsl_brick
./render_wrl.sh scenes/sphere.x3dv -s rsl_checker
./render_wrl.sh scenes/sphere.x3dv -s rsl_cyclone
./render_wrl.sh scenes/sphere.x3dv -s rsl_cyclone2
./render_wrl.sh scenes/sphere.x3dv -s rsl_glass
./render_wrl.sh scenes/sphere.x3dv -s rsl_glass2
./render_wrl.sh scenes/sphere.x3dv -s rsl_granite
./render_wrl.sh scenes/sphere.x3dv -s rsl_lava2
./render_wrl.sh scenes/sphere.x3dv -s rsl_meta
./render_wrl.sh scenes/sphere.x3dv -s rsl_parquetplank
./render_wrl.sh scenes/sphere.x3dv -s rsl_phong
./render_wrl.sh scenes/sphere.x3dv -s rsl_plastic
./render_wrl.sh scenes/sphere.x3dv -s rsl_screen
./render_wrl.sh scenes/sphere.x3dv -s rsl_siggraph10_tv
./render_wrl.sh scenes/sphere.x3dv -s rsl_starball
./render_wrl.sh scenes/sphere.x3dv -s rsl_venus
./render_wrl.sh scenes/sphere.x3dv -s rsl_whitted
./render_wrl.sh scenes/sphere.x3dv -s rsl_wood
echo ""
echo "TESTING OF SCALAR RSL SHADERS FINISHED!"
echo ""
