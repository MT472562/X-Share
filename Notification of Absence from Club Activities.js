const TOKEN = PropertiesService.getScriptProperties().getProperty("LINE_TOKEN");

const doPost = (e) => {
  const replyToken = JSON.parse(e.postData.contents).events[0].replyToken;
  const userMessage = JSON.parse(e.postData.contents).events[0].message.text;
  const groupId = JSON.parse(e.postData.contents).events[0].source.groupId;
  const url = 'https://api.line.me/v2/bot/message/reply';
  const Permission_Groups = PropertiesService.getScriptProperties().getProperty("Permission_Groups");
  var array = JSON.parse(Permission_Groups);
  if (userMessage == '集計') {
    if (array.includes(groupId)) {
      var s_url = PropertiesService.getScriptProperties().getProperty("SHEET_URL");
      var sheetId = s_url.match(/\/d\/([a-zA-Z0-9-_]+)\//)[1];
      var sourceSpreadsheet = SpreadsheetApp.openById(sheetId);
      var sourceSheet = sourceSpreadsheet.getSheetByName('Answer');

      var today = new Date();
      var formattedDate = Utilities.formatDate(today, Session.getScriptTimeZone(), 'yyyy/MM/dd');
      Logger.log(formattedDate);
      var count = 0;
      var data = sourceSheet.getDataRange().getValues();
      var text = "おはようございます。\n欠席連絡です。\n\n----------------\n\n"
      for (var i = 1; i < data.length; i++) {
        let get_date = JSON.stringify(data[i][3]).replace(/"/g, '');
        if (String(get_date) == String(formattedDate)) {
          Logger.log(data[i][1] + data[i][2]);
          var text = text + "名前：" + data[i][1] + "さん\n理由：" + data[i][2] + "\n\n";
          var count = count + 1;
        }
      }
      var text = text + "お休みされる人数" + count + "人\n\n----------------\n日付:" + formattedDate + "\nよろしくお願いします。";
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
    } else {
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
              'text': "グループID認証が確認できませんでした。くわしくは[ヘルプ]と入力してください",
            }],
          }),
        });
      return response.getResponseCode();
    }
  } else if (userMessage == 'グループID認証を開始') {
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
            'text': "Please register the group ID as an environment variable in the script\n\n" + groupId,
          }],
        }),
      });
    return response.getResponseCode();
  } else if (userMessage == 'ヘルプ') {
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
            'text': "集計を行う場合[集計]と入力してください。\n 入力した日付に関する欠席情報を集計して出力をします。\n\n欠席連絡フォームのURLはLINEノートからアクセスしてください。\n\n 登録がされていないグループは[グループID認証を開始]と入力して返答されたグループIDをシステムに登録してください。",
          }],
        }),
      });
    return response.getResponseCode();
  }
};
