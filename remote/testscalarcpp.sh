echo "TESTING SCALAR SHADERS..."
./render_wrl.sh scenes/sphere.x3dv -s brick
./render_wrl.sh scenes/sphere.x3dv -s checker
./render_wrl.sh scenes/sphere.x3dv -s glass
./render_wrl.sh scenes/sphere.x3dv -s granite
./render_wrl.sh scenes/sphere.x3dv -s mandelbrot
./render_wrl.sh scenes/sphere.x3dv -s parquetplank
./render_wrl.sh scenes/sphere.x3dv -s phong
./render_wrl.sh scenes/sphere.x3dv -s screenaa
./render_wrl.sh scenes/sphere.x3dv -s starball
./render_wrl.sh scenes/sphere.x3dv -s whitted
./render_wrl.sh scenes/sphere.x3dv -s wood
echo ""
echo "TESTING OF SCALAR SHADERS FINISHED!"
echo ""
