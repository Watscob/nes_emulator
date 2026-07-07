NES emulator (support Linux native and web)

## Linux native
### Build
```sh
./build.sh -b
# OR
./build_docker.sh -b
```

### Run
```sh
build/src/nesemulator <rom_file>
# OR
./run_native_docker.sh <rom_file>
```

### Unit tests
```sh
build/tests/test_nesemulator
```

## Web assembly
### Build
```sh
./build.sh -bw
# OR
./build_docker.sh -bw
```

### Run
```sh
python3 -m http.server -b localhost 8000 -d web/
```
Then, open http://localhost:8000
