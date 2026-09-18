import Base
import ./types.bend as Types
import ./cachet.bend as Cachet

def Access.sub(+c: Types.Claims) -> String:
  match c:
    case Types.C{sub, iss, aud, exp, iat}:
      sub

def Access.iss(+c: Types.Claims) -> String:
  match c:
    case Types.C{sub, iss, aud, exp, iat}:
      iss

def Access.aud(+c: Types.Claims) -> String:
  match c:
    case Types.C{sub, iss, aud, exp, iat}:
      aud

def Access.exp(+c: Types.Claims) -> U32:
  match c:
    case Types.C{sub, iss, aud, exp, iat}:
      exp

def Access.iat(+c: Types.Claims) -> U32:
  match c:
    case Types.C{sub, iss, aud, exp, iat}:
      iat

def Json.field_str(+key: String, +value: String) -> String:
  String.append(
    String.append(String.append(String.append("\"", key), "\":\""), value),
    "\"")

def Json.field_u32(+key: String, value: U32) -> String:
  String.append(String.append(String.append("\"", key), "\":"), U32.show(value))

def Json.encode(+c: Types.Claims) -> String:
  String.append(
    String.append(
      String.append(
        String.append(
          String.append("{", Json.field_str("sub", Access.sub(c))),
          ","),
        Json.field_str("iss", Access.iss(c))),
      String.append(
        String.append(",", Json.field_str("aud", Access.aud(c))),
        String.append(",", Json.field_u32("exp", Access.exp(c))))),
    String.append(String.append(",", Json.field_u32("iat", Access.iat(c))), "}"))

def Parse.from_fields(
  sub: String,
  iss: String,
  aud: String,
  exp: U32,
  iat: U32
) -> Types.Claims:
  Types.C{sub, iss, aud, exp, iat}

def Parse.pipe(+packed: String) -> IO(Result<&1, &1, U32 & String, Types.Claims>):
  do IO<Result<&1, &1, U32 & String, Types.Claims>>:
    sub : String <- IO.try(String, Cachet.Pipe.field(packed, 0))
    iss : String <- IO.try(String, Cachet.Pipe.field(packed, 1))
    aud : String <- IO.try(String, Cachet.Pipe.field(packed, 2))
    exp_s : String <- IO.try(String, Cachet.Pipe.field(packed, 3))
    iat_s : String <- IO.try(String, Cachet.Pipe.field(packed, 4))
    exp : U32 <- IO.try(U32, Cachet.Codec.u32_of_string(exp_s))
    iat : U32 <- IO.try(U32, Cachet.Codec.u32_of_string(iat_s))
    return Done{Parse.from_fields(sub, iss, aud, exp, iat)}

def Parse.raw(+json: String) -> IO(Result<&1, &1, U32 & String, String>):
  import "./effs/claims_parse.c"
  import "./effs/claims_parse.js"

def Parse.json(+json: String) -> IO(Result<&1, &1, U32 & String, Types.Claims>):
  do IO<Result<&1, &1, U32 & String, Types.Claims>>:
    packed : String <- IO.try(String, Parse.raw(json))
    parsed : Result<&1, &1, U32 & String, Types.Claims> <- Parse.pipe(packed)
    return parsed

def Validate.auth_code() -> U32:
  401

def Validate.is_expired(+c: Types.Claims, now: U32) -> Bool:
  U32.is_lt(Access.exp(c), now)

def Validate.when_expired(flag: Bool) -> Result<&1, &1, U32 & String, Unit>:
  match flag:
    case True{}:
      Fail{(Validate.auth_code(), "cachet: token expired")}
    case False{}:
      Done{Unit{}}

def Validate.when_issuer(flag: Bool) -> Result<&1, &1, U32 & String, Unit>:
  match flag:
    case True{}:
      Done{Unit{}}
    case False{}:
      Fail{(Validate.auth_code(), "cachet: invalid issuer")}

def Validate.when_audience(flag: Bool) -> Result<&1, &1, U32 & String, Unit>:
  match flag:
    case True{}:
      Done{Unit{}}
    case False{}:
      Fail{(Validate.auth_code(), "cachet: invalid audience")}

def Validate.check_issuer(+c: Types.Claims, +expected: String) -> Result<&1, &1, U32 & String, Unit>:
  Validate.when_issuer(String.eq(Access.iss(c), expected))

def Validate.check_audience(+c: Types.Claims, +expected: String) -> Result<&1, &1, U32 & String, Unit>:
  Validate.when_audience(String.eq(Access.aud(c), expected))

def Validate.check_expiry(+c: Types.Claims, now: U32) -> Result<&1, &1, U32 & String, Unit>:
  Validate.when_expired(Validate.is_expired(c, now))

def Validate.after_unit(
  step: Result<&1, &1, U32 & String, Unit>,
  next: Result<&1, &1, U32 & String, Types.Claims>
) -> Result<&1, &1, U32 & String, Types.Claims>:
  match step:
    case Fail{err}:
      Fail{err}
    case Done{_}:
      next

def Validate.audience_step(
  +c: Types.Claims,
  +expected_aud: String,
  now: U32,
  issuer: Result<&1, &1, U32 & String, Unit>
) -> Result<&1, &1, U32 & String, Types.Claims>:
  Validate.after_unit(
    issuer,
    Validate.after_unit(
      Validate.check_audience(c, expected_aud),
      Validate.after_unit(Validate.check_expiry(c, now), Done{c})))

def Validate.run(
  +c: Types.Claims,
  +expected_iss: String,
  +expected_aud: String,
  now: U32
) -> Result<&1, &1, U32 & String, Types.Claims>:
  Validate.audience_step(c, expected_aud, now, Validate.check_issuer(c, expected_iss))
