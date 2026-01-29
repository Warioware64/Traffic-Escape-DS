#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py
PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv
GRIT=/opt/wonderful/thirdparty/blocksds/core/tools/grit/grit

echo "=========================================="
echo "Converting Mesh Files (OBJ -> BIN)"
echo "=========================================="

# Loop through all .obj files in the Meshes folder
for obj_file in Meshes/*.obj; do
    # Check if file exists (in case no .obj files are found)
    if [ ! -f "$obj_file" ]; then
        echo "No .obj files found in Meshes/"
        exit 1
    fi

    # Extract filename without path and extension
    filename=$(basename "$obj_file" .obj)

    echo "Converting: $filename.obj -> $filename.bin"

    # Run obj2dl with specified parameters
    $OBJ2DL \
        --input "$obj_file" \
        --output "../nitrofiles/Vehicules/Meshes/${filename}.bin" \
        --scale 0.5 \
        --texture 128 128

    # Check if conversion was successful
    if [ $? -eq 0 ]; then
        echo "✓ Successfully converted $filename.obj"
    else
        echo "✗ Failed to convert $filename.obj"
        exit 1
    fi
done

echo ""
echo "All OBJ files converted successfully!"
echo ""

echo "=========================================="
echo "Converting Texture Files (PNG -> TEX/PAL)"
echo "=========================================="

# Loop through all .png files in the Textures folder
for png_file in Textures/*.png; do
    # Check if file exists (in case no .png files are found)
    if [ ! -f "$png_file" ]; then
        echo "No .png files found in Textures/"
        exit 1
    fi

    # Extract filename without path and extension
    filename=$(basename "$png_file" .png)

    echo "Converting: $filename.png -> $filename (tex/pal)"

    # Run ptexconv with specified parameters
    $PTEXCONV \
        -gt \
        -ob \
        -o "../nitrofiles/Vehicules/Textures/${filename}" \
        -f tex4x4 \
        "$png_file"

    cat ../nitrofiles/Vehicules/Textures/${filename}_tex.bin ../nitrofiles/Vehicules/Textures/${filename}_idx.bin > ../nitrofiles/Vehicules/Textures/${filename}_combined.bin
    rm ../nitrofiles/Vehicules/Textures/${filename}_tex.bin ../nitrofiles/Vehicules/Textures/${filename}_idx.bin
    # Check if conversion was successful
    if [ $? -eq 0 ]; then
        echo "✓ Successfully converted $filename.png"
    else
        echo "✗ Failed to convert $filename.png"
        exit 1
    fi
done

echo ""
echo "All PNG files converted successfully!"
echo ""
echo "=========================================="
echo "Conversion Complete!"
echo "=========================================="



echo "=========================================="
echo "Converting Texture Files (PNG -> BG)"
echo "=========================================="

# Loop through all .png files in the Textures folder
for png_file in BGs/*.png; do
    # Check if file exists (in case no .png files are found)
    if [ ! -f "$png_file" ]; then
        echo "No .png files found in Textures/"
        exit 1
    fi

    # Extract filename without path and extension
    filename=$(basename "$png_file" .png)

    echo "Converting: $filename.png -> $filename (tex/pal)"

    # Run ptexconv with specified parameters
    $GRIT \
        "$png_file" \
        -gt \
        -gB8 \
        -mR8 \
        -mLs \
        -gTFFOOFF \
        -ftb \
        -fh! \
        -o "../nitrofiles/BGs/${filename}"


    # Check if conversion was successful
    if [ $? -eq 0 ]; then
        echo "✓ Successfully converted $filename.png"
    else
        echo "✗ Failed to convert $filename.png"
        exit 1
    fi
done

echo ""
echo "All PNG files converted successfully!"
echo ""
echo "=========================================="
echo "Conversion Complete!"
echo "=========================================="