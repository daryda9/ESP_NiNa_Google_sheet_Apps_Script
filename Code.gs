function doGet(e) {
  var temperatura = e.parameter.temperatura;
  var umidita = e.parameter.umidita;

  Logger.log(e);
  
  var sheet = SpreadsheetApp.openById('-- Incolla ID spreadsheet --').getSheetByName("DATI");
  
  // Aggiungi una nuova riga con i dati ricevuti
  sheet.appendRow([new Date(), temperatura, umidita]);

  // Trova l'ultima riga con dati
  var lastRow = sheet.getLastRow();

  // Leggi la cella a riga 2, colonna 5 .getRange(riga, colonna).getValue()
  var maxTemperatura = sheet.getRange(2, 5).getValue();
  
  var ret = {
    status: 'success',
    maxTemperatura: maxTemperatura
  };

  return ContentService.createTextOutput(JSON.stringify(ret)).setMimeType(ContentService.MimeType.JSON);
}

