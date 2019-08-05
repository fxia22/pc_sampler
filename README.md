## Build
```
mkdir build
cd build
cmake ..
make 
```

## Usage
```
 ./pc_sampler ~/Downloads/Allensville/mesh_z_up.obj <optional points per square meter> > pc.xyz
 python xyz2ply.py --infile build/pc.xyz --outfile test.ply <optional bounding box information> 
 ```