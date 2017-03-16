echo "TESTING SHADERS..."
./render_wrl.sh scenes/sphere_boxplane.x3dv -s brick -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s brick -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s checker -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s checker -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s dented -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s dented -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s glass -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s glass -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s granite -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s granite -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s mirror -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s mirror -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s mandelbrot -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s mandelbrot -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s parquetplank -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s parquetplank -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s phong -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s phong -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s screenaa -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s screenaa -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s starball -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s starball -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s whitted -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s whitted -s checker_reflective -p

./render_wrl.sh scenes/sphere_boxplane.x3dv -s wood -s checker_reflective
./render_wrl.sh scenes/sphere_boxplane.x3dv -s wood -s checker_reflective -p
echo ""
echo "TESTING OF SHADERS FINISHED!"
echo ""
