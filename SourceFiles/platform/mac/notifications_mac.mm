//
// Created by Michael Neonov on 16.07.2023.
//

#include "platform/platform_notifications.h"

#import <UserNotifications/UserNotifications.h>

@interface NotificationDelegate : NSObject <UNUserNotificationCenterDelegate> {}

@end

@implementation NotificationDelegate

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler {
    completionHandler(UNNotificationPresentationOptionAlert);
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
didReceiveNotificationResponse:(UNNotificationResponse *)response
         withCompletionHandler:(void(^)())completionHandler {
    completionHandler();
}

@end

namespace Platform::Notifications {
    bool Supported() {
        return false;
    }

    bool Init() {
        return false;
    }

    void askNotification(const QString &text, Fn<void(bool)> callback) {
        callback(true);
    }

    void infoNotificaiton(const QString& text, Fn<void()> onClick) {

    }
} // namespace Platform::Notifications
