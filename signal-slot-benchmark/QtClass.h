#pragma once

#include <QObject>

class QCaller : public QObject
{
	Q_OBJECT

public:
	void run(int iVal)
	{
		emit invoke(iVal);
	}

signals:
	void invoke(int iVal);
};

class QReciver : public QObject
{
	Q_OBJECT

public slots:
	void impl(int iVal)
	{
		m_iAnswer = 0;
		for (int i = 0; i < iVal; ++i)
			m_iAnswer += i;
	}

public:
	int m_iAnswer;
};