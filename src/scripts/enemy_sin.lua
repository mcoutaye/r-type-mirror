-- Script de test pour un ennemi : Ajoute un mouvement sinus custom, et tire aléatoirement

function onInit(self, entity)
    print("[Lua] Init pour entity " .. entity)
    self.amplitude = 100  -- Variable locale à cette instance (plus grand que le pattern C++)
    self.fireChance = 0.05  -- 5% chance de tir par frame
    self.cooldown = 0  -- Cooldown interne pour tir
end

function onUpdate(self, dt, entity)
    local pos = getPosition(entity)
    local vel = getVelocity(entity)
    local health = getHealth(entity)

    if pos and vel then
        -- Custom sinus sur Y (override/add au MovementSystem C++)
        pos.y = pos.y + math.sin(os.time() * 5) * self.amplitude * dt  -- Oscille plus vite/amplifié

        -- Accélère si low HP
        if health and health.current < health.max / 2 then
            vel.x = vel.x - 50 * dt  -- Plus rapide vers la gauche
        end

        -- Tir aléatoire (crée un projectile enemy)
        self.cooldown = self.cooldown - dt
        if self.cooldown <= 0 and math.random() < self.fireChance then
            createProjectile(ECS, pos.x - 20, pos.y, -400, 0, 2, 10, "bullet_enemy")  -- Tir vers gauche, team enemy
            self.cooldown = 1.0  -- 1s cooldown
        end
    end

    -- Flag pour réseau (si modifié)
    local update = getSendUpdate(entity)
    if update then
        update.needsUpdate = true
    end
end

function onCollision(self, otherEntity)
    print("[Lua] Collision de " .. entity .. " avec " .. otherEntity)
    -- Exemple : Inflige extra dégâts
    local otherHealth = getHealth(otherEntity)
    if otherHealth then
        otherHealth.current = otherHealth.current - 5
    end
end