import { Button, styled } from "@material-ui/core";

const ErrorButton = styled(Button)(({ theme }) => ({
  color: theme.palette.getContrastText(theme.palette.error.main),
  backgroundColor: theme.palette.error.main,
  '&:hover': {
    backgroundColor: theme.palette.error.dark,
  }
}));

export default ErrorButton;
