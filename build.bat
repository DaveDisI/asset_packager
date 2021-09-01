echo off
cls
IF %1 == system_build (
    cl /Zi -nologo asset_packager.cpp
)
IF %1 == application_run (
    asset_packager C:/Users/Dave/Desktop/code/btl_png/assets btl_png_asset_index.h btl_png.paf
)