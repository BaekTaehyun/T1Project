namespace DataTemplate
{

public enum eGuild_customType : int
{
    emblemFrame = 0,
    emblemSymbol = 1,
    emblemColor = 2,
    max = 3,
}
public enum eGuild_balanceType : int
{
    guildCreationCost = 0,
    emblemChangeCost = 1,
    expPerAttendance = 2,
    guildCoinPerAttendance = 3,
    guildCoinPerPrism = 4,
    reentranceTimeLimit = 5,
    maxRequestJoin = 6,
    maxApplicant = 7,
    maxGuildName = 8,
    minGuildName = 9,
    maxGuildIntro = 10,
    maxGuildNotice = 11,
    expPerPrism = 12,
    AttendanceCoolTime = 13,
    RewardDefaultCommoner = 14,
    RewardDefaultSergeant = 15,
    RewardDefaultCaptain = 16,
    RewardDefaultColonel = 17,
    RewardDefaultLeader = 18,
    RewardMaxAllot = 19,
    AttendanceResetTime = 20,
    Max = 21,
}
public enum eGuild_memberType : int
{
    commoner = 10,
    sergeant = 20,
    captain = 30,
    colonel = 40,
    leader = 50,
    max = 99,
}
public enum eGuild_accessCondition : int
{
    online = 0,
    offline = 1,
    max = 2,
}
public enum eGuild_joinType : int
{
    requestJoin = 0,
    freeJoin = 1,
    max = 2,
}
public enum eGuild_historyType : int
{
    none = 0,
    growthHistory = 1,
    memberHistory = 2,
    donationHistory = 3,
    activityHistory = 4,
    rewardHistory = 5,
    max = 6,
}
public enum eGuild_historyContents : int
{
    none = 0,
    guildLevel = 1,
    buffLevel = 2,
    newcomer = 3,
    seceder = 4,
    typeChange = 5,
    attendance = 6,
    donation = 7,
    guildRanking = 8,
    specialShop = 9,
    RewardAllot = 10,
    MonarchReward = 11,
    RemainingDia = 12,
    max = 13,
}
public enum eGuild_HelpType : int
{
    GuildLevelInfo = 0,
    GuildContentsOpen = 1,
    GuildBuffOpen = 2,
    GuildShopOpen = 3,
    GuildCapacity = 4,
    max = 5,
}
}
