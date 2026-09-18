import Base

type Claims is Data:
  C{sub: String, iss: String, aud: String, exp: U32, iat: U32}

type Secret is Data:
  S{value: String}

type Token is Data:
  T{value: String}

type TokenParts is Data:
  P{head: String, payload: String, sig: String}

type AuthError is Data:
  Err{code: U32, message: String}

def Types.auth_code() -> U32:
  401

def Types.bad_request() -> U32:
  400
