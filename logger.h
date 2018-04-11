
#ifndef LOGGER_H
#define LOGGER_H
#include <QDebug>
#include <QString>
#include <QVector>
#include <QChar>

class Logger
{
	public:
		static uint indent;
		QVector<QString> strs;
		Logger& operator<<(const QString& s){
			put(s);
			return *this;
		}
		void put(const QString& str){
			//Q_UNUSED(str)
			strs.append(str);
		}

		Logger& operator<<(char c){
			put(QString(QChar(c)));
			return *this;
		}

		Logger& operator<<(const char* arr){
			put(QString(arr));
			return *this;
		}

		Logger& operator<<(int i){
			put(QString::number(i));
			return *this;
		}

		~Logger(){
			if (!strs.isEmpty()){
				QDebug deb = qDebug().noquote();
				for (uint i = 0; i < indent; i++)
					deb << "   ";
				for (int i = 0; i != strs.size(); i++){
					deb << strs.at(i);
				}
			}
		}
};
#define Log Logger()
#define LFn_ Log << __PRETTY_FUNCTION__ << "\n"
#define LFn LFn_; Logger::indent++; Log
#define LRet return Logger::indent--, Log << __PRETTY_FUNCTION__ << "returned",
#define LVoid Logger::indent--; Log << __PRETTY_FUNCTION__ << "returned";
#define LVd LVoid
#define LThw Logger::indent--; Log << __PRETTY_FUNCTION__ << "threw"; throw
#define LCThw(STMTS,TY) \
	try{\
STMTS\
} catch(TY & t){LThw t;}
#endif // LOGGER_H
