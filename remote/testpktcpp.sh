echo "TESTING PACKETIZED SHADERS..."
./render_wrl.sh scenes/sphere.x3dv -s brick -p
./render_wrl.sh scenes/sphere.x3dv -s checker -p
./render_wrl.sh scenes/sphere.x3dv -s glass -p
./render_wrl.sh scenes/sphere.x3dv -s granite -p
./render_wrl.sh scenes/sphere.x3dv -s mandelbrot -p
./render_wrl.sh scenes/sphere.x3dv -s parquetplank -p
./render_wrl.sh scenes/sphere.x3dv -s phong -p
./render_wrl.sh scenes/sphere.x3dv -s screenaa -p
./render_wrl.sh scenes/sphere.x3dv -s starball -p
./render_wrl.sh scenes/sphere.x3dv -s whitted -p
./render_wrl.sh scenes/sphere.x3dv -s wood -p
echo ""
echo "TESTING OF PACKETIZED SHADERS FINISHED!"
echo ""
