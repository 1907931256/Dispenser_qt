#include "workflow.h"

Workflow::Workflow(QObject *parent) : QObject(parent)
{
	setIOStatus();
	setConfig();

	setThread();
}

void Workflow::setConfig()
{
	// ��������
	QFile file("../config/workflow_glue.ini");
	if (!file.exists())
	{
		is_config_gluel = false;
		is_config_glue2 = false;
		is_config_glue3 = false;
	}
	else
	{
		QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
		is_config_gluel = setting.value("workflow_glue/is_config_gluel").toBool();
		is_config_glue2 = setting.value("workflow_glue/is_config_glue2").toBool();
		is_config_glue3 = setting.value("workflow_glue/is_config_glue3").toBool();
	}
}

void Workflow::setIOStatus()
{
	if (!(init_card() == 1)) return;

	// ��1�� ��ʼ�����״̬

	// ��2�� �������״̬
}

void Workflow::setThread()
{
	// qDebug() << QThreadPool::globalInstance()->maxThreadCount();

	thread_pool.setMaxThreadCount(6);

	is_start_ok = false;
	start_thread_watch_start = false;
	close_thread_watch_start = false;

	is_reset_ok = false;
	start_thread_watch_reset = false;
	close_thread_watch_reset = false;

	is_stop_ok = false;
	close_thread_watch_stop = false;
	close_thread_watch_stop = false;

	is_estop_ok = false;
	close_thread_watch_estop = false;
	close_thread_watch_estop = false;

	is_workflow_ok = false;
	start_thread_workflow = false;
	close_thread_workflow = false;
	
	is_exchangeTrays_ok = false;
	start_thread_exchangeTrays = false;
	close_thread_exchangeTrays = false;
	
	future_thread_watch_start = QtConcurrent::run(&thread_pool, [&]() { thread_watch_start(); });
	future_thread_watch_reset = QtConcurrent::run(&thread_pool, [&]() { thread_watch_reset(); });
	future_thread_watch_stop  = QtConcurrent::run(&thread_pool, [&]() { thread_watch_stop(); });
	future_thread_watch_estop = QtConcurrent::run(&thread_pool, [&]() { thread_watch_estop(); });
	
	future_thread_workflow = QtConcurrent::run(&thread_pool, [&]() { thread_workflow(); });
	future_thread_exchangeTrays = QtConcurrent::run(&thread_pool, [&]() { thread_exchangeTrays(); });
}

// Thread ����
void Workflow::thread_watch_start()
{
	if (!(init_card() == 1)) return;

	while (close_thread_watch_start == false)
	{
		if (adt8949_read_bit(0, 17) == 1)
		{
			// ��ʼ
			emit changedRundataLabel(QStringLiteral("������..."));
			emit changedRundataText(QStringLiteral("��ʼ����"));
			writRunningLog(QStringLiteral("��ʼ����"));

			// �����㽺�߳�

			break;
		}
	}


}

// Thread ��λ
void Workflow::thread_watch_reset()
{
	if (!(init_card() == 1)) return;

	int step_reset = 0;

	while (close_thread_watch_reset == false)
	{
		if (adt8949_read_bit(0, 20) == 1)
		{
			switch (step_reset)
			{
			case 0:	// ��λ��ʼ
				{
					// ��Ϣ����
					emit changedRundataLabel(QStringLiteral("��λ��ʼ..."));
					emit changedRundataText(QStringLiteral("��λ��ʼ"));
					writRunningLog(QStringLiteral("��λ��ʼ"));

					step_reset = 10;
					break;
				}

			case 10: // �������
				{
					emit changedRundataLabel(QStringLiteral("��������ź�"));
					emit changedRundataText(QStringLiteral("��������ź�"));
					writRunningLog(QStringLiteral("��������ź�"));
					step_reset = 20;
					break;
				}

			case 20: // ��ʼ�����
				{
					// ��ʼ�����

					emit changedRundataLabel(QStringLiteral("��ʼ�����"));
					emit changedRundataText(QStringLiteral("��ʼ�����"));
					writRunningLog(QStringLiteral("��ʼ�����"));
					step_reset = 30;
					break;
				}

			case 30: // �������, ���
				{
					// �������, ���
					emit changedRundataLabel(QStringLiteral("����������"));
					emit changedRundataText(QStringLiteral("����������"));
					writRunningLog(QStringLiteral("����������"));
				
					step_reset = 40;
					break;
				}

			case 40: // ��վ��λ
				{ 
					emit changedRundataLabel(QStringLiteral("��վ��λ"));
					emit changedRundataText(QStringLiteral("��վ��λ"));
					writRunningLog(QStringLiteral("��վ��λ"));

					// ��1�� ��վ��λ
					adt8949_HomeProcess_Ex(0, 3);	// Z���ȸ�λ
					wait_finish(3);

					adt8949_HomeProcess_Ex(0, 1);
					adt8949_HomeProcess_Ex(0, 2);
					wait_finish(1);
					wait_finish(2);

					// ��2�� �жϸ�λ״̬
					emit changedRundataLabel(QStringLiteral("��վ��λ���"));
					emit changedRundataText(QStringLiteral("��վ��λ���"));
					writRunningLog(QStringLiteral("��վ��λ���"));

					step_reset = 50;
					break;
				}

			case 50: // ��λ���
				{
					emit changedRundataLabel(QStringLiteral("��λ���, �Ѿ���"));
					emit changedRundataText(QStringLiteral("��λ���"));
					writRunningLog(QStringLiteral("��λ���"));

					break;
				}
			
			default:
				break;
			}

			break; 
		}
	}
}

// Thread ֹͣ
void Workflow::thread_watch_stop()
{
	
}

// Thread ��ͣ
void Workflow::thread_watch_estop()
{
	if (!(init_card() == 1)) return;

	while (close_thread_watch_estop == false)
	{
		if (adt8949_read_bit(0, 16) == 1)
		{
			// ��ͣ
			stop_allaxis();
			estop();
			break;
		}
	}
}


// Thread ��������
void Workflow::thread_workflow()
{
	if (!(init_card() == 1)) return;

	int step_workflow = 0;

	while (close_thread_workflow == false)
	{
		switch (step_workflow)
		{
		case 0: // ���̿�ʼ
		{
			emit changedRundataText(QStringLiteral("�������̿�ʼ"));
			writRunningLog(QStringLiteral("�������̿�ʼ"));

			step_workflow = 10;
			break;
		}

		case 10: // 
		{
			break;
		}

		case 20:
		{
			break;
		}

		case 30:
		{
			break;
		}

		default:
			break;
		}
	}
	
}


// Thread ������
void Workflow::thread_exchangeTrays()
{
	if (!(init_card() == 1)) return;

	int step_tray;

	while (close_thread_exchangeTrays == false)
	{
		switch (step_tray)
		{
		case 0:		// �ȴ�����
			{
				if (start_thread_exchangeTrays == false)
				{
					step_tray = 0;
				}
				else
				{
					// �����̿�ʼ
					emit changedRundataText(QStringLiteral("�����̿�ʼ"));
					writRunningLog(QStringLiteral("�����̿�ʼ"));

					step_tray = 10;
				}
			}
			break;

		case 10:	// �����˳�
			{
				if (1 == read_in_bit(33))	// ���̵�λ��Ӧ
				{
					write_out_bit(15, 0);	// �������Ƴ�
					Sleep(2000);

					step_tray = 20;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���̵�λ��Ӧ�쳣"));

					step_tray = 9999;
				}
			}
			break;

		case 20:	// ��Ӧ�����Ƿ��˳��ɹ�
			{
				if (1 == read_in_bit(34))	// �����˳���λ��Ӧ
				{
					step_tray = 30;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���̵�λ��Ӧ�쳣"));

					step_tray = 9999;
				}
			}

		case 30:    // �ȴ�����ȡ��, 
			{
				if (1 == read_in_bit(35))	// ����δȡ��
				{
					step_tray == 30;
				}
				else
				{
					Sleep(2000);	// ��ֹȡ�º�, �ַŻ�
					step_tray = 40;
				}
				break;
			}

		case 40:	// �ȴ���������װ��
			{
				if (1 == read_in_bit(35))
				{
					Sleep(3000);	// �ȴ����뿪����
					step_tray = 50;
				}
				else
				{
					step_tray = 40;
				}
			}
			break;

		case 50:	// �������ƹ�ȥ
			{
				write_out_bit(15, 1);
				Sleep(1000);

				step_tray = 8888;
			}
			break;

		case 8888:
			{
				emit changedRundataText(QStringLiteral("�����̽���"));
				writRunningLog(QStringLiteral("�����̽���"));
				start_thread_exchangeTrays = false;
				step_tray = 0;
			}
			break;

		case 9999:
			{
				// ��ȫ�˳����߳�
				close_thread_exchangeTrays = true;

				// ����ֹͣ�ź�
				
			}
			break;

		default:
			break;
		}
	}
}

void Workflow::thread_glue_1()
{

}

void Workflow::thread_glue_2()
{

}

void Workflow::thread_glue_3()
{

}


// �Զ����, �����ⲿ�ź�
void Workflow::on_changedConfigGlue()
{
	QFile file("../config/workflow_glue.ini");
	if (!file.exists()) return;
	else
	{
		QSettings setting("../config/workflow_glue.ini", QSettings::IniFormat);
		is_config_gluel = setting.value("workflow_glue/is_config_gluel").toBool();
		is_config_glue2 = setting.value("workflow_glue/is_config_glue2").toBool();
		is_config_glue3 = setting.value("workflow_glue/is_config_glue3").toBool();
	}
}


// дlog�ļ�
void Workflow::writRunningLog(QString str)
{
	// ��1�� �����Ϣ��ʽ��
	QDateTime currentDate = QDateTime::currentDateTime();
	QString s_currentDate = currentDate.toString(QStringLiteral("yyyy-MM-dd"));
	QString s_currentTime = getCurrentTime();
	QString s_filepath = QString("../data/log/%1.txt").arg(s_currentDate);
	QString s_write = s_currentTime + "   " + str + "\n";

	// ��2�� ���ļ���д������
	QFile file(s_filepath);
	file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	QTextStream in(&file);
	in << s_write;
	file.close();
}

// ��ȡ��ǰʱ��
QString Workflow::getCurrentTime()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString s_currentTime = currentTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
	return s_currentTime;
}