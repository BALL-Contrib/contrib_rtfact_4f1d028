echo "RUNNING SHADER DEMO..."

# teapot_box_vp1: shd0 = teapot, shd1 = box
# teapot_box_vp2: shd0 = teapot, shd1 = box
# teapot_box_vp3: shd0 = teapot, shd1 = box

# sphere_box_vp1: shd0 = sphere, shd1 = box
# sphere_box_vp2: shd0 = sphere, shd1 = box

# box: shd0 = box, shd1 = unused

rtsgRTremoteTest scenes/AnySL_Columns.wrl 
rtsgRTremoteTest scenes/AnySL_Columns_pkt.wrl
rtsgRTremoteTest scenes/AnySL_Columns_CameraAnimationComparison.wrl

./render_wrl.sh scenes/teapot_box_vp1.x3dv -s glass -s checker
./render_wrl.sh scenes/teapot_box_vp1.x3dv -s glass -s checker -p

./render_wrl.sh scenes/teapot_box_vp2.x3dv -s phong -s glass
./render_wrl.sh scenes/teapot_box_vp2.x3dv -s phong -s glass -p

./render_wrl.sh scenes/teapot_box_vp3.x3dv -s whitted -s brick
./render_wrl.sh scenes/teapot_box_vp3.x3dv -s whitted -s brick -p

./render_wrl.sh scenes/sphere_box_vp2.x3dv -s starball_glass -s parquetplank
./render_wrl.sh scenes/sphere_box_vp2.x3dv -s starball_glass -s parquetplank -p

./render_wrl.sh scenes/sphere_box_vp1.x3dv -s screenaa -s granite
./render_wrl.sh scenes/sphere_box_vp1.x3dv -s screenaa -s granite -p

./render_wrl.sh scenes/box.x3dv -s mandelbrot
#./render_wrl.sh scenes/box.x3dv -s mandelbrot -p #something wrong here :p

./render_wrl.sh ../../../../models/wrl/f030.wrl -s checker
./render_wrl.sh ../../../../models/wrl/f030.wrl -s checker -p

./render_wrl.sh ../../../../models/wrl/bun_zipper.wrl -s phong -p

./render_wrl.sh ../../../../models/wrl/dragon_vrip.wrl -s brick -p

./render_wrl.sh ../../../../models/wrl/happy_vrip.wrl -s wood -p


echo ""
echo "SHADER DEMO FINISHED!"
echo ""
