lib_LTLIBRARIES = libchannelcore.la
dist_bin_SCRIPTS = rpc-channel-server.py
dist_data_DATA = rpc-stubs/channel-gsp-rpc.json
gamechanneldir = $(includedir)/gamechannel
rpcstubdir = $(gamechanneldir)/rpc-stubs
protodir = $(gamechanneldir)/proto
pydir = $(pythondir)/gamechannel
pyprotodir = $(pydir)/proto

pkgconfig_DATA = channelcore.pc

RPC_STUBS = \
  rpc-stubs/channelgsprpcclient.h \
  rpc-stubs/channelgsprpcserverstub.h \
  rpc-stubs/rpcbroadcastclient.h

PROTOS = \
  proto/broadcast.proto \
  proto/metadata.proto \
  proto/testprotos.proto \
  proto/signatures.proto \
  proto/stateproof.proto
PROTOHEADERS = $(PROTOS:.proto=.pb.h)
PROTOSOURCES = $(PROTOS:.proto=.pb.cc)
PROTOPY = $(PROTOS:.proto=_pb2.py)

EXTRA_DIST = $(PROTOS) \
  rpc-stubs/rpcbroadcast.json

BUILT_SOURCES = $(RPC_STUBS) $(PROTOHEADERS) $(PROTOPY)
CLEANFILES = $(RPC_STUBS) $(PROTOHEADERS) $(PROTOSOURCES) $(PROTOPY) schema.cpp

libchannelcore_la_CXXFLAGS = \
  -I$(top_srcdir) \
  $(JSONCPP_CFLAGS) $(ETHUTILS_CFLAGS) $(GLOG_CFLAGS) $(PROTOBUF_CFLAGS)
libchannelcore_la_LIBADD = \
  $(top_builddir)/xayautil/libxayautil.la \
  $(JSONCPP_LIBS) $(ETHUTILS_LIBS) $(GLOG_LIBS) $(PROTOBUF_LIBS)
libchannelcore_la_SOURCES = \
  boardrules.cpp \
  broadcast.cpp \
  channelmanager.cpp \
  channelstatejson.cpp \
  ethsignatures.cpp \
  movesender.cpp \
  openchannel.cpp \
  protoversion.cpp \
  rollingstate.cpp \
  signatures.cpp \
  stateproof.cpp \
  $(PROTOSOURCES)
CHANNELCOREHEADERS = \
  boardrules.hpp \
  broadcast.hpp \
  channelmanager.hpp channelmanager.tpp \
  channelstatejson.hpp \
  ethsignatures.hpp \
  movesender.hpp \
  openchannel.hpp \
  protoboard.hpp protoboard.tpp \
  protoutils.hpp protoutils.tpp \
  protoversion.hpp \
  rollingstate.hpp \
  signatures.hpp \
  stateproof.hpp



gamechannel_HEADERS = $(CHANNELCOREHEADERS)
rpcstub_HEADERS = $(RPC_STUBS)
proto_HEADERS = $(PROTOHEADERS)

PYTHONTESTS = \
  signatures_tests.py \
  test_rpcbroadcast.py
noinst_PYTHON = $(PYTHONTESTS)
py_PYTHON = \
  __init__.py \
  channeltest.py \
  rpcbroadcast.py \
  signatures.py
pyproto_PYTHON = proto/__init__.py $(PROTOPY)

AM_TESTS_ENVIRONMENT = \
  PYTHONPATH=$(top_srcdir)

rpc-stubs/channelgsprpcclient.h: $(srcdir)/rpc-stubs/channel-gsp-rpc.json
	jsonrpcstub "$<" --cpp-client=ChannelGspRpcClient --cpp-client-file="$@"
rpc-stubs/channelgsprpcserverstub.h: $(srcdir)/rpc-stubs/channel-gsp-rpc.json
	jsonrpcstub "$<" \
	  --cpp-server=ChannelGspRpcServerStub \
	  --cpp-server-file="$@"

rpc-stubs/rpcbroadcastclient.h: $(srcdir)/rpc-stubs/rpcbroadcast.json
	jsonrpcstub "$<" --cpp-client=RpcBroadcastClient --cpp-client-file="$@"

proto/%.pb.h proto/%.pb.cc: $(top_srcdir)/gamechannel/proto/%.proto
	protoc -I$(top_srcdir) --cpp_out=$(top_builddir) "$<"
proto/%_pb2.py: $(top_srcdir)/gamechannel/proto/%.proto
	protoc -I$(top_srcdir) --python_out=$(top_builddir) "$<"
