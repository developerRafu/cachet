import Base

def Codec.base64url_encode(input: String) -> IO(String):
  import "./effs/base64url_encode.c"
  import "./effs/base64url_encode.js"

def Codec.base64url_decode(input: String) -> IO(String):
  import "./effs/base64url_decode.c"
  import "./effs/base64url_decode.js"

def Crypto.hmac_sha256(message: String, secret: String) -> IO(String):
  import "./effs/hmac_sha256.c"
  import "./effs/hmac_sha256.js"

def Clock.unix_now() -> IO(U32):
  import "./effs/unix_now.c"
  import "./effs/unix_now.js"

def Pipe.field(+packed: String, index: U32) -> IO(Result<&1, &1, U32 & String, String>):
  import "./effs/pipe_field.c"
  import "./effs/pipe_field.js"

def Codec.u32_of_string(+text: String) -> IO(Result<&1, &1, U32 & String, U32>):
  import "./effs/u32_of_string.c"
  import "./effs/u32_of_string.js"
