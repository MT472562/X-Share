const TOKEN = 'LINE-TOKEN';

const doPost = (e) => {
  const replyToken = JSON.parse(e.postData.contents).events[0].replyToken;
  const userMessage = JSON.parse(e.postData.contents).events[0].message.text;
  const url = 'https://api.line.me/v2/bot/message/reply';

  if (userMessage == '集計') {
    var s_url = 'フォーム集計URL';
    var sheetId = s_url.match(/\/d\/([a-zA-Z0-9-_]+)\//)[1];
    var sourceSpreadsheet = SpreadsheetApp.openById(sheetId);
    var sourceSheet = sourceSpreadsheet.getSheetByName('Answer');
    var today = new Date();
    var formattedDate = Utilities.formatDate(today, Session.getScriptTimeZone(), 'yyyy/MM/dd');
    Logger.log(formattedDate);
    var count =0;
    var data = sourceSheet.getDataRange().getValues();
    var text = "本日お休みされる方を集計しました。\n\n----------------\n\n"
    for (var i = 1; i < data.length; i++) {
      let get_date = JSON.stringify(data[i][3]).replace(/"/g, '');
      if (String(get_date) == String(formattedDate)) {
        Logger.log(data[i][1] + data[i][2]);
        var text = text + "名前：" + data[i][1] + "さん\n理由：" + data[i][2] + "\n\n";
        var count = count+1;
      }
    }
    var text = text + "お休みされる人数" + count + "人\n\n----------------\n集計結果は以上になります。\nこの集計はプログラムによって自動生成されています。\n日付:" + formattedDate;
    const response =
      UrlFetchApp.fetch(url, {
        'headers': {
          'Content-Type': 'application/json; charset=UTF-8',
          'Authorization': 'Bearer ' + TOKEN,
        },
        'method': 'post',
        'payload': JSON.stringify({
          'replyToken': replyToken,
          'messages': [{
            'type': 'text',
            'text': text,
          }],
        }),
      });
    return response.getResponseCode();
  }
};
