echo off
%~dp0protoc.exe -I=./ --cpp_out=./../ ./ProtoMessages.proto