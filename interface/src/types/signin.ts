export interface SignInRequest {
  username: string;
  password: string;
}

export interface SignInResponse {
  access_token: string;
}
