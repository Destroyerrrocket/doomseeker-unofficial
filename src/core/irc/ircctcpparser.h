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
			DisplayThroughGlobalMessage,
			DontShow
		};

		enum MessageType
		{
			/**
			 * @brief A question is being asked through PRIVMSG.
			 */
			Send,
			/**
			 * @brief An answer is being sent through NOTICE.
			 */
			Reply
		};

		IRCCtcpParser(IRCNetworkAdapter *network, const QString &sender,
			const QString &recipient, const QString &msg, MessageType msgType);
		~IRCCtcpParser();

		CtcpEcho echo() const;
		bool parse();
		const QString &printable() const;
		const QString &reply() const;

	private:
		class PrivData;
		PrivData *d;

		bool isCommand(const QString &candidate);
		bool isCtcp() const;
		/**
		 * @brief Fill d->command and d->params.
		 */
		void tokenizeMsg();
};


#endif
