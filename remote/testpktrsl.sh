echo "TESTING PACKETIZED RSL SHADERS..."
./render_wrl.sh scenes/sphere.x3dv -s rsl_normal -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_bw -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_diffuse -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_specular -p

./render_wrl.sh scenes/sphere.x3dv -s rsl_brick -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_checker -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_cyclone -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_cyclone2 -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_glass -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_glass2 -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_granite -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_lava2 -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_metal -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_parquetplank -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_phong -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_plastic -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_screen -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_siggraph10_tv -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_starball -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_venus -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_whitted -p
./render_wrl.sh scenes/sphere.x3dv -s rsl_wood -p
echo ""
echo "TESTING OF PACKETIZED RSL SHADERS FINISHED!"
echo ""
