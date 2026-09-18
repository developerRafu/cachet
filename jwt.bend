import Base
import ./types.bend as Types
import ./cachet.bend as Cachet
import ./claims.bend as Claims

def JWT.auth_code() -> U32:
  401

def JWT.bad_request() -> U32:
  400

def JWT.header_json() -> String:
  "{\"alg\":\"HS256\",\"typ\":\"JWT\"}"

def JWT.dot() -> String:
  "."

def JWT.dot_char() -> Char:
  '.'

def JWT.token_parts(+token: String) -> IO(Result<&1, &1, U32 & String, String>):
  import "./effs/token_parts.c"
  import "./effs/token_parts.js"

def JWT.parts_from_packed(+packed: String) -> IO(Result<&1, &1, U32 & String, Types.TokenParts>):
  do IO<Result<&1, &1, U32 & String, Types.TokenParts>>:
    head : String <- IO.try(String, Cachet.Pipe.field(packed, 0))
    payload : String <- IO.try(String, Cachet.Pipe.field(packed, 1))
    sig : String <- IO.try(String, Cachet.Pipe.field(packed, 2))
    return Done{Types.P{head, payload, sig}}

def JWT.sig_eq(got: String, expected: String) -> Bool:
  String.eq(got, expected)

def JWT.join_signing(head_b64: String, pay_b64: String) -> String:
  String.append(String.append(head_b64, JWT.dot()), pay_b64)

def JWT.join_token(signing: String, sig: String) -> String:
  String.append(String.append(signing, JWT.dot()), sig)

def JWT.when_sig(flag: Bool) -> Result<&1, &1, U32 & String, Unit>:
  match flag:
    case True{}:
      Done{Unit{}}
    case False{}:
      Fail{(JWT.auth_code(), "cachet: invalid signature")}

def JWT.verify_sig(sig: String, signing: String, +secret: String) ->
  IO(Result<&1, &1, U32 & String, Unit>):
  do IO<Result<&1, &1, U32 & String, Unit>>:
    expected : String <- Cachet.Crypto.hmac_sha256(signing, secret)
    return JWT.when_sig(JWT.sig_eq(sig, expected))

def JWT.invalid_format() -> Result<&1, &1, U32 & String, String>:
  Fail{(JWT.bad_request(), "cachet: invalid token format")}

def JWT.head_of(+parts: Types.TokenParts) -> String:
  match parts:
    case Types.P{head, payload, sig}:
      head

def JWT.payload_of(+parts: Types.TokenParts) -> String:
  match parts:
    case Types.P{head, payload, sig}:
      payload

def JWT.sig_of(+parts: Types.TokenParts) -> String:
  match parts:
    case Types.P{head, payload, sig}:
      sig

def JWT.verify_signed(+parts: Types.TokenParts, +secret: String) ->
  IO(Result<&1, &1, U32 & String, String>):
  do IO<Result<&1, &1, U32 & String, String>>:
    signing : String = JWT.join_signing(JWT.head_of(parts), JWT.payload_of(parts))
    _ : Unit <- IO.try(Unit, JWT.verify_sig(JWT.sig_of(parts), signing, secret))
    payload : String <- Cachet.Codec.base64url_decode(JWT.payload_of(parts))
    return Done{payload}

def Verify.payload(+token: String, +secret: String) -> IO(Result<&1, &1, U32 & String, String>):
  import "./effs/verify_token.c"
  import "./effs/verify_token.js"

def Decode.claims(+token: String, +secret: String) ->
  IO(Result<&1, &1, U32 & String, Types.Claims>):
  do IO<Result<&1, &1, U32 & String, Types.Claims>>:
    payload : String <- IO.try(String, Verify.payload(token, secret))
    parsed : Result<&1, &1, U32 & String, Types.Claims> <- Claims.Parse.json(payload)
    return parsed

def Sign.token_raw(+payload: String, +secret: String) -> IO(Result<&1, &1, U32 & String, String>):
  import "./effs/sign_token.c"
  import "./effs/sign_token.js"

def Sign.token(+c: Types.Claims, +secret: String) ->
  IO(Result<&1, &1, U32 & String, String>):
  Sign.token_raw(Claims.Json.encode(c), secret)
