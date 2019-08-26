$(document).ready(function() {
  FungsiAjax(null, 'https://q1l9e3e3g1.execute-api.us-east-1.amazonaws.com/kabuka/riwayat');
  setInterval(function () {
    FungsiAjax(null, 'https://q1l9e3e3g1.execute-api.us-east-1.amazonaws.com/kabuka/riwayat');
  }, 60000);
});