#include "MainTable.h"
#include <handy/handy.h>
#include "json/json.h"
#include "svm.h"
#include <regex>
// using namespace std;
// using namespace handy;

class ParamsUrl
{
public:
  ParamsUrl(const string &url) : _url(url) {}
  ParamsUrl(string &&url) : _url(move(url)) {}
  string Get(const string &action) const;

private:
  string _url;
};

string ParamsUrl::Get(const string &request) const
{
  smatch result;
  if (regex_search(_url.cbegin(), _url.cend(), result, regex(request + "=(.*?)&")))
  {
    return result[1];
  }
  else if (regex_search(_url.cbegin(), _url.cend(), result, regex(request + "=(.*)")))
  {
    return result[1];
  }
  else
  {
    return string();
  }
}

void resetdata(Json::Value &answers, AnswerTable &answer)
{
  std::vector<std::vector<AnswerCell>> answerCells = answer.answerCells;
  string row_name;
  if (answer.answerCells.size() == 0)
  {
    answers["code"] = 400;
    answers["error"] = "no search answer";
  }
  answers["rows"] = answer.rows;
  for (int i = 0; i < answer.answerCells.size(); i++)
  {
    std::vector<AnswerCell> rowCells = answer.answerCells[i];
    row_name = "row_" + std::to_string(i);
    answers[row_name] = Json::Value(Json::arrayValue);
    for (int j = 0; j < rowCells.size(); j++)
    {
      AnswerCell cell = rowCells[j];
      Json::Value v;
      v["row"] = cell.row;
      v["col"] = cell.col;
      v["rect"]["x"] = cell.rect.x;
      v["rect"]["y"] = cell.rect.y;
      v["rect"]["width"] = cell.rect.width;
      v["rect"]["height"] = cell.rect.height;
      v["isManual"] = cell.isManual;

      v["chars"] = Json::Value(Json::arrayValue);
      v["probs"] = Json::Value(Json::arrayValue);
      for (int c_size = 0; c_size < cell.chars.size(); c_size++)
      {
        string _answer_char(1, cell.chars[c_size]);
        v["chars"][c_size] = _answer_char;
        v["probs"][c_size] = cell.probs[c_size];
      }

      answers[row_name].append(v);
    }
  }
}

int main(int argc, const char *argv[])
{
  //4线程时，100并发，31失败，90%线程响应时间约 20s
  int threads = 4;
  if (argc > 1)
  {
    threads = atoi(argv[1]);
  }
  // setloglevel("TRACE");
  handy::MultiBase base(threads);
  handy::HttpServer sample(&base);
  int r = sample.bind("0.0.0.0", 8081);
  cout << "server start" << endl;
  static svm_model *svm_model_loaded = svm_load_model("./model/libsvm.model");
  // handy::exitif(r, "bind failed %d %s", errno, handystrerror(errno));
  sample.onGet("/readimage", [](const handy::HttpConnPtr &con) {
    Json::FastWriter writer;
    std::string v = con.getRequest().version;
    std::string uri = con.getRequest().query_uri;
    ParamsUrl params(uri);
    cout << uri << endl;
    Json::Value answers;
    string url = params.Get("url");
    handy::HttpResponse resp;
    if (url == "")
    {
      cout << "no url" << endl;
      answers["code"] = 400;
      answers["error"] = "no url params";
    }
    else
    {
      answers["code"] = 0;
      AnswerTable answer = analysisUrl(url, svm_model_loaded);

      if (answer.imgRight == false)
      {
        answers["code"] = 401;
        answers["error"] = "img read error";
      }
      else if (answer.imgRight == false)
      {
        answers["code"] = 401;
        answers["error"] = "table read error";
      }
      else
      {
        cout << "行数为: " << answer.rows << endl;
        resetdata(answers, answer);
      }
    }
    resp.body = handy::Slice(writer.write(answers));
    con.sendResponse(resp);
    if (v == "HTTP/1.0" || v == "HTTP/1.1")
    {
      con->close();
    }
  });
  handy::Signal::signal(SIGINT, [&] {
    base.exit();
    //svm model free 20180521
    svm_free_model_content(svm_model_loaded);
  });
  base.loop();
  return 0;
}
