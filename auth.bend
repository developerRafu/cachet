import Base
import ./types.bend as Types
import ./claims.bend as Claims
import ./jwt.bend as JWT
import ./cachet.bend as Cachet

def Auth.auth_code() -> U32:
  401

def Auth.bearer_prefix() -> String:
  "Bearer "

def Auth.prefix_len() -> Nat:
  7n

def Auth.has_bearer(+header: String) -> Bool:
  String.starts_with(header, Auth.bearer_prefix())

def Auth.when_bearer_maybe(flag: Bool, +header: String) -> Maybe<String>:
  match flag:
    case True{}:
      Some{String.drop(header, Auth.prefix_len())}
    case False{}:
      None{}

def Auth.bearer_token(+header: String) -> Maybe<String>:
  Auth.when_bearer_maybe(Auth.has_bearer(header), header)

def Auth.when_bearer(flag: Bool, +header: String) -> Result<&1, &1, U32 & String, String>:
  match flag:
    case True{}:
      Done{String.drop(header, Auth.prefix_len())}
    case False{}:
      Fail{(Auth.auth_code(), "cachet: missing bearer token")}

def Auth.token_from_header(+header: String) -> Result<&1, &1, U32 & String, String>:
  Auth.when_bearer(Auth.has_bearer(header), header)

def Guard.authorize(
  +header: String,
  +secret: String,
  +expected_iss: String,
  +expected_aud: String
) -> IO(Result<&1, &1, U32 & String, Types.Claims>):
  do IO<Result<&1, &1, U32 & String, Types.Claims>>:
    token : String <- IO.pass(String, Auth.token_from_header(header))
    claims : Types.Claims <- IO.try(Types.Claims, JWT.Decode.claims(token, secret))
    now : U32 <- Cachet.Clock.unix_now()
    validated : Result<&1, &1, U32 & String, Types.Claims> =
      Claims.Validate.run(claims, expected_iss, expected_aud, now)
    return validated

def Auth.on_authorized(+claims: Types.Claims) -> IO(Result<&1, &1, U32 & String, Unit>):
  do IO<Result<&1, &1, U32 & String, Unit>>:
    IO.print(Claims.Access.sub(claims))
    return Done{Unit{}}
