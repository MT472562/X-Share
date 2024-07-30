function createList() {
  const form = FormApp.getActiveForm();
  const item = form.getItems();
  var day = new Date();

  let choices = [];
  const daysOfWeek = ['日', '月', '火', '水', '木', '金', '土'];

  for (let i = 0; i < 7; i++) {
    let currentDate = new Date(day.getFullYear(), day.getMonth(), day.getDate() + i);
    let dayStr = Utilities.formatString(
      "%04d/%02d/%02d (%s)", 
      currentDate.getFullYear(), 
      currentDate.getMonth() + 1, 
      currentDate.getDate(),
      daysOfWeek[currentDate.getDay()]
    );
    choices.push(dayStr);
  }

  // 設問番号を入力　例　3個目の設問なら2番（0カウントスタート）
  item[0].asListItem().setChoiceValues(choices);
}
