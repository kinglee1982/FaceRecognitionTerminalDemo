#!/bin/bash

set -e

build_type="Release"
cmake_generator="Unix Makefiles"

source_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
build_root_dir="$source_dir/build"
clean_3rd="no"

for i in "$@"; do
    case $i in
    -d=* | --build-dir=*)
        build_root_dir="${i#*=}"
        shift # past argument=value
        ;;
    -t=* | --build-type=*)
        build_type="${i#*=}"
        shift # past argument=value
        ;;
    --ninja)
        cmake_generator="Ninja"
        shift # past argument with no value
        ;;
    --clean-3rd)
        clean_3rd="yes"
        shift # past argument with no value
        ;;
    *)
        # unknown option
        ;;
    esac
done

build_dir=$build_root_dir/$build_type
dep_dir=$source_dir/deps
install_dir=$build_dir/install

dep_sub_dirs="qufacesdk 3rd rp-dv300"
if [[ $clean_3rd == "yes" ]]; then
    echo "Cleaning dep dir ..."
    for d in $dep_sub_dirs; do
        rm -rf $source_dir/deps/$d || true
        echo "Clean dir: $source_dir/deps/$d"
    done
    echo "Cleaning dep dir done"
else
    for d in $dep_sub_dirs; do
        if [ -d $source_dir/deps/$d ]; then
            echo "[WARN] '$source_dir/deps/$d' exists, won't update"
        fi
    done
fi

mkdir -p $build_dir || true
pushd $build_dir
cmake $source_dir -G "$cmake_generator" \
    -DCMAKE_INSTALL_PREFIX=$install_dir \
    -DHISI_SDK_PLATFORM=rp-dv300 \
    -DDOWNLOAD_DEPENDENCY=ON \
    -DPROJECT_DEPENDENCY_DIR=$dep_dir \
    -DCMAKE_TOOLCHAIN_FILE=$source_dir/cmake/himix200.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=$build_type
cmake --build . -- -j 4
cmake --build . --target install
popd

echo -e "\n#####################################################\n"
echo -e "请将下面的文件夹内容拷贝至开发板后，手动运行程序即可: "
echo -e "\t $install_dir"
ls -lh $install_dir
echo -e "\n#####################################################\n"
