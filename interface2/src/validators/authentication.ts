import Schema from 'async-validator';

export const SIGN_IN_REQUEST_VALIDATOR = new Schema({
  username: {
    required: true, message: "Please provide a username"
  },
  password: {
    required: true, message: "Please provide a password"
  }
});
