# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
import grpc

import spider_pb2 as spider__pb2


class ScheduleStub(object):
  # missing associated documentation comment in .proto file
  pass

  def __init__(self, channel):
    """Constructor.

    Args:
      channel: A grpc.Channel.
    """
    self.add_task = channel.unary_unary(
        '/spiderproto.Schedule/add_task',
        request_serializer=spider__pb2.BasicTask.SerializeToString,
        response_deserializer=spider__pb2.TaskResponse.FromString,
        )
    self.add_fetcher = channel.unary_unary(
        '/spiderproto.Schedule/add_fetcher',
        request_serializer=spider__pb2.Fetcher.SerializeToString,
        response_deserializer=spider__pb2.TaskResponse.FromString,
        )
    self.add_crawledtask = channel.unary_unary(
        '/spiderproto.Schedule/add_crawledtask',
        request_serializer=spider__pb2.CrawledTask.SerializeToString,
        response_deserializer=spider__pb2.TaskResponse.FromString,
        )


class ScheduleServicer(object):
  # missing associated documentation comment in .proto file
  pass

  def add_task(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')

  def add_fetcher(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')

  def add_crawledtask(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')


def add_ScheduleServicer_to_server(servicer, server):
  rpc_method_handlers = {
      'add_task': grpc.unary_unary_rpc_method_handler(
          servicer.add_task,
          request_deserializer=spider__pb2.BasicTask.FromString,
          response_serializer=spider__pb2.TaskResponse.SerializeToString,
      ),
      'add_fetcher': grpc.unary_unary_rpc_method_handler(
          servicer.add_fetcher,
          request_deserializer=spider__pb2.Fetcher.FromString,
          response_serializer=spider__pb2.TaskResponse.SerializeToString,
      ),
      'add_crawledtask': grpc.unary_unary_rpc_method_handler(
          servicer.add_crawledtask,
          request_deserializer=spider__pb2.CrawledTask.FromString,
          response_serializer=spider__pb2.TaskResponse.SerializeToString,
      ),
  }
  generic_handler = grpc.method_handlers_generic_handler(
      'spiderproto.Schedule', rpc_method_handlers)
  server.add_generic_rpc_handlers((generic_handler,))


class FetchStub(object):
  # missing associated documentation comment in .proto file
  pass

  def __init__(self, channel):
    """Constructor.

    Args:
      channel: A grpc.Channel.
    """
    self.add_crawlingtask = channel.unary_unary(
        '/spiderproto.Fetch/add_crawlingtask',
        request_serializer=spider__pb2.CrawlingTask.SerializeToString,
        response_deserializer=spider__pb2.TaskResponse.FromString,
        )
    self.Ping = channel.unary_unary(
        '/spiderproto.Fetch/Ping',
        request_serializer=spider__pb2.PingRequest.SerializeToString,
        response_deserializer=spider__pb2.PingResponse.FromString,
        )


class FetchServicer(object):
  # missing associated documentation comment in .proto file
  pass

  def add_crawlingtask(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')

  def Ping(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')


def add_FetchServicer_to_server(servicer, server):
  rpc_method_handlers = {
      'add_crawlingtask': grpc.unary_unary_rpc_method_handler(
          servicer.add_crawlingtask,
          request_deserializer=spider__pb2.CrawlingTask.FromString,
          response_serializer=spider__pb2.TaskResponse.SerializeToString,
      ),
      'Ping': grpc.unary_unary_rpc_method_handler(
          servicer.Ping,
          request_deserializer=spider__pb2.PingRequest.FromString,
          response_serializer=spider__pb2.PingResponse.SerializeToString,
      ),
  }
  generic_handler = grpc.method_handlers_generic_handler(
      'spiderproto.Fetch', rpc_method_handlers)
  server.add_generic_rpc_handlers((generic_handler,))


class HandleStub(object):
  # missing associated documentation comment in .proto file
  pass

  def __init__(self, channel):
    """Constructor.

    Args:
      channel: A grpc.Channel.
    """
    self.add_crawldoc = channel.unary_unary(
        '/spiderproto.Handle/add_crawldoc',
        request_serializer=spider__pb2.CrawlDoc.SerializeToString,
        response_deserializer=spider__pb2.TaskResponse.FromString,
        )
    self.Ping = channel.unary_unary(
        '/spiderproto.Handle/Ping',
        request_serializer=spider__pb2.PingRequest.SerializeToString,
        response_deserializer=spider__pb2.PingResponse.FromString,
        )


class HandleServicer(object):
  # missing associated documentation comment in .proto file
  pass

  def add_crawldoc(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')

  def Ping(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')


def add_HandleServicer_to_server(servicer, server):
  rpc_method_handlers = {
      'add_crawldoc': grpc.unary_unary_rpc_method_handler(
          servicer.add_crawldoc,
          request_deserializer=spider__pb2.CrawlDoc.FromString,
          response_serializer=spider__pb2.TaskResponse.SerializeToString,
      ),
      'Ping': grpc.unary_unary_rpc_method_handler(
          servicer.Ping,
          request_deserializer=spider__pb2.PingRequest.FromString,
          response_serializer=spider__pb2.PingResponse.SerializeToString,
      ),
  }
  generic_handler = grpc.method_handlers_generic_handler(
      'spiderproto.Handle', rpc_method_handlers)
  server.add_generic_rpc_handlers((generic_handler,))
