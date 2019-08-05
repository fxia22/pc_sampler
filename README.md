## Build
```
mkdir build
cd build
cmake ..
make 
```

## Usage
```
 ./pc_sampler ~/Downloads/Allensville/mesh_z_up.obj <optional: points per square meter> > pc.xyz
 python xyz2ply.py --infile build/pc.xyz --outfile test.ply <optional: bounding box information> 
 ```


## Cut and remeshing pipeline

```
./pc_sampler ~/Downloads/Allensville/mesh_z_up.obj <optional: points per square meter> > pc.xyz
python xyz2ply.py --infile pc.xyz --outfile pc_color.ply <bounding box information> 
./PoissonRecon --in pc_color.ply --out poisson_no_color.ply --depth 10 --density
./SurfaceTrimmer --in poisson_no_color.ply --out poisson_trimmed.ply --trim 7
meshlabserver -i poisson_no_color.ply pc_color.ply -s color_transfer.mlx -o poisson_color.ply -om vn vc 
```