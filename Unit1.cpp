//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <map>
 
//========================================================================
// вывод массива-результата
inline void showMass (const std::vector<int>& mass)
{ std::copy(mass.cbegin(), mass.cend(), std::ostream_iterator<int>(std::cout, " "));
 
//========================================================================
// вывод матрицы
template <typename T>
void showMatr (const T& matr, const size_t& columns)
{
  for (size_t j = 0, sz = matr.size() / columns; j != sz; ++j)
  {
    for (size_t k = 0; k != columns; ++k)
      std::cout << std::setw(3) << matr[j * columns + k] << " ";
    std::cout << std::endl;
  }
}
 
//========================================================================
// избавляемся от ситуации, когда какую-то утку сбить нельзя вообще, нас такие не интересуют
bool isDataCorrect (const std::vector<bool>& shoots, const size_t& hunters)
{
  for (auto it = shoots.cbegin(), itEnd = shoots.cend(); it != itEnd; it += hunters)
    if (std::find(it, it + hunters, true) == it + hunters)
      return false;
  return true;
}
 
//========================================================================
// проверка на реальность комбинации
bool isComboPossible (
                       const std::vector<bool>& shoots,
                       const std::vector<int>& combo
                     )
{
  for (size_t j = 0, sz = combo.size(); j != sz; ++j)
    if (!shoots[j * shoots.size() / combo.size() + combo[j]])
      return false;
  return true;
}
 
//========================================================================
std::vector<int> calcResult (
                              const std::vector<bool>& shoots,
                              std::vector<size_t>& times,
                              const size_t& hunters
                            )
{
  const size_t ducks = shoots.size() / hunters;
  std::vector<int> res(ducks, 0),
                   temp = res;
 
  size_t minTime = -1,
         maxDeadDucks = 0;
 
  int cnt;
  do
  {
    cnt = ducks - 1;
    std::map<size_t, int> hunterAndDuck;
 
    if (isComboPossible(shoots, temp))
    {
      for (size_t j = 0, sz = temp.size(); j != sz; ++j)
      {
        auto it = hunterAndDuck.find(temp[j]);
        if (it != hunterAndDuck.end())
        {
          if (times[j * hunters + temp[j]] <
              times[it->second * hunters + temp[j]])
            hunterAndDuck[temp[j]] = j;
        }
        else
          hunterAndDuck[temp[j]] = j;
      }
 
      size_t curTime = 0;
      for (const auto& j : hunterAndDuck)
        curTime += times[j.second * hunters + j.first];
 
      if (hunterAndDuck.size() > maxDeadDucks ||
          (hunterAndDuck.size() == maxDeadDucks && curTime < minTime))
      {
        res = temp;
        minTime = curTime;
        maxDeadDucks = hunterAndDuck.size();
 
        for (size_t j = 0; j != hunters; ++j)
        {
          auto it = hunterAndDuck.find(j);
          if (it != hunterAndDuck.cend())
          {
            for (size_t k = 0, sz = res.size(); k != sz; ++k)
              if (res[k] == it->first &&
                  k != it->second)
                res[k] = -1;
          }
        }
      }
    }
 
      while (cnt >= 0 &&
           ++temp[cnt] > static_cast<int>(hunters - 1))
      {
      temp[cnt] = 0;
      --cnt;
      }
  }
  while (cnt >= 0);
 
  return res;
}
 
//========================================================================
int main ()
{
  std::default_random_engine dre{std::random_device()()};
  std::uniform_int_distribution<size_t> distr(2, 5);
 
  size_t ducks = distr(dre),
         hunters = distr(dre);
 
  distr = std::uniform_int_distribution<size_t>(1, 15);
 
  // исходные данные
  std::vector<bool> shoots(ducks * hunters, false);              // для каждой утки - какой охотник ее божет сбить
  std::vector<size_t> times(ducks * hunters, 0);                 // для каждой утки - время, за которое ее собьет охотник
 
  do
  {
    std::generate(shoots.begin(), shoots.end(),
                  std::bind(std::bernoulli_distribution(), dre));
  }
  while (!isDataCorrect(shoots, hunters));  
 
  for (size_t j = 0, sz = shoots.size(); j != sz; ++j)
    if (shoots[j])
      times[j] = distr(dre);
 
  // вывод исходных данных
  std::cout << "Shoots:\n";
  showMatr(shoots, hunters);
  std::cout << std::endl;
 
  std::cout << "Times:\n";
  showMatr(times, hunters);
  std::cout << std::endl;
 
  /*
   *   результат
   *   для каждой утки - номер охотника, который ее собъет (отсчет от 0)
   *   -1 - утку никто не сбил
  */
 
  std::cout << "Result:\n";
  showMass(calcResult(shoots, times, hunters));
  std::cout << std::endl;
 
  return 0;
}


