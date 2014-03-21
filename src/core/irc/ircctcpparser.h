#ifndef id14670887_AF0E_4EAD_9A495B9E413C6723
#define id14670887_AF0E_4EAD_9A495B9E413C6723

#include <QObject>
#include <QString>

class IRCNetworkAdapter;

class IRCCtcpParser : public QObject
{
	Q_OBJECT

	public:
		enum CtcpEcho
		{
			PrintAsNormalMessage,
			DisplayInServerTab,
			DontShow
		};

		IRCCtcpParser(IRCNetworkAdapter *network, const QString &sender,
			const QString &recipient, const QString &msg);
		~IRCCtcpParser();

		CtcpEcho echo() const;
		bool parse();
		QString printable() const;

	private:
		class PrivData;
		PrivData *d;

		bool isCommand(const QString &candidate);
		bool isCtcp() const;
		void reply(const QString &what);
		/**
		 * @brief Fill d->command and d->params.
		 */
		void tokenizeMsg();
};


#endif
