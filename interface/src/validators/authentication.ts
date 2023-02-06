import Schema from 'async-validator';

export const SIGN_IN_REQUEST_VALIDATOR = new Schema({
  username: {
    required: true
  },
  password: {
    required: true
  }
});
